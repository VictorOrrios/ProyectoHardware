/**
 * @file svc_stats.c
 * @ingroup SVC_STATS
 * @brief Statistics Service Implementation
 * @details Implementation of the statistics service module. Collects and processes
 *          various system metrics for performance analysis and debugging.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "svc_stats.h"
#include "svc_log.h"
#include "drv_tiempo.h"
#include "rt_fifo.h"
#include "rt_evento_t.h"
#include "drv_sc.h"
#include <string.h> // For memset
#include <stdint.h>

#ifdef DEBUG_STATS

/******************************************************************************
 * Data Structures
 *****************************************************************************/

/* FIFO Statistics */
typedef struct {
    uint32_t total_events;
    uint32_t events_by_type[EVENT_TYPES];
    uint32_t unhandled_events;
    uint32_t events_by_type_unhandled[EVENT_TYPES];  // Events that timed out
} fifo_stats_t;

/* Queue Configuration */
#define TIMING_QUEUE_SIZE 64  // Should be a power of 2

/* Generic Timing Queue */
typedef struct {
    Tiempo_us_t start_times[TIMING_QUEUE_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} timing_queue_t;

/* Response Time Statistics */
typedef struct {
    // Interrupt timing
    timing_queue_t interrupt_queue;
    Tiempo_us_t interrupt_total_time;
    Tiempo_us_t interrupt_max_time;
    Tiempo_us_t interrupt_min_time;
    Tiempo_us_t interrupt_avg_time;
    uint32_t interrupt_count;

    // Wait mode timing
    timing_queue_t wait_queue;
    Tiempo_us_t wait_total_time;
    Tiempo_us_t wait_max_time;
    Tiempo_us_t wait_min_time;
    Tiempo_us_t wait_avg_time;
    uint32_t wait_cycles;

    // Sleep mode timing
    timing_queue_t sleep_queue;
    Tiempo_us_t sleep_total_time;
    Tiempo_us_t sleep_max_time;
    Tiempo_us_t sleep_min_time;
    Tiempo_us_t sleep_avg_time;
    uint32_t sleep_cycles;

    // User response timing
    timing_queue_t user_queue;
    Tiempo_us_t user_total_time;
    Tiempo_us_t user_max_time;
    Tiempo_us_t user_min_time;
    Tiempo_us_t user_avg_time;
    uint32_t user_response_count;

    // FIFO timing
    timing_queue_t fifo_queue;
    Tiempo_us_t fifo_total_time;
    Tiempo_us_t fifo_max_time;
    Tiempo_us_t fifo_min_time;
    Tiempo_us_t fifo_avg_time;
} timing_stats_t;

/* Power Statistics */
typedef struct {
    Tiempo_us_t total_active_time;
    Tiempo_us_t total_sleep_time;
    uint32_t sleep_cycles;
    uint32_t wakeups;
} power_stats_t;

/* Game Statistics Configuration */
#define MAX_SEQUENCE_LENGTH 32
#define MAX_DIFFICULTY_LEVEL 3

/* Game Statistics */
typedef struct {
    uint32_t total_games;
    uint32_t max_sequence;
    uint32_t min_sequence;
    uint32_t sequences_histogram[MAX_SEQUENCE_LENGTH];  // Count of sequences by length (0 to 31)
    float avg_sequence;
    uint32_t difficulty_histogram[MAX_DIFFICULTY_LEVEL];  // Count of games by difficulty (1 to 3)
    Tiempo_us_t current_game_start;    // Start time of current game
    Tiempo_us_t max_game_duration;     // Longest game duration
    Tiempo_us_t min_game_duration;     // Shortest game duration
    Tiempo_us_t avg_game_duration;     // Average game duration
} game_stats_t;

/******************************************************************************
 * Private Helper Functions
 *****************************************************************************/

/**
 * @brief Push a timestamp into a timing queue
 * @param time Timestamp to store
 * @param queue Pointer to the queue structure
 * @note [DEPRECATED] Logs error and discards value on queue overflow
 */
static void queue_push(Tiempo_us_t time, timing_queue_t *queue)
{
    if (queue->count >= TIMING_QUEUE_SIZE)
    {
        // CRITICAL: causes race condition & prefetch abort in the log service
        /*LOG_INFO("Timing queue overflow!");
        svc_log_procesar();*/
        return;
    }
    queue->start_times[queue->head] = time;
    queue->head = (queue->head + 1) & (TIMING_QUEUE_SIZE - 1);
    queue->count++;
}

/**
 * @brief Pop a timestamp from a timing queue
 * @param queue Pointer to the queue structure
 * @return The timestamp, or 0 if queue is empty
 * @note [DEPRECATED] Logs error and discards value on queue overflow
 */
static Tiempo_us_t queue_pop(timing_queue_t *queue)
{
    if (queue->count == 0)
    {
        // CRITICAL: causes race condition & prefetch abort in the log service
        /*LOG_INFO("Timing queue underflow!");
        svc_log_procesar();*/
        return 0;
    }
    Tiempo_us_t time = queue->start_times[queue->tail];
    queue->tail = (queue->tail + 1) & (TIMING_QUEUE_SIZE - 1);
    queue->count--;
    return time;
}

/******************************************************************************
 * Private Variables
 *****************************************************************************/

static fifo_stats_t fifo_stats;
static timing_stats_t timing_stats;
static power_stats_t power_stats;
static game_stats_t game_stats;

/******************************************************************************
 * Initialization
 *****************************************************************************/
void svc_stats_iniciar(void)
{
    // Use memset for cleaner initialization
    memset(&fifo_stats, 0, sizeof(fifo_stats));
    memset(&timing_stats, 0, sizeof(timing_stats));
    memset(&power_stats, 0, sizeof(power_stats));
    memset(&game_stats, 0, sizeof(game_stats));

    // Initialize min values using UINT32_MAX
    timing_stats.interrupt_min_time = UINT32_MAX;
    timing_stats.wait_min_time = UINT32_MAX;
    timing_stats.sleep_min_time = UINT32_MAX;
    timing_stats.user_min_time = UINT32_MAX;
    timing_stats.fifo_min_time = UINT32_MAX;
    game_stats.min_sequence = UINT32_MAX;
    game_stats.min_game_duration = UINT32_MAX;
}

/******************************************************************************
 * Game Statistics
 *****************************************************************************/

/**
 * @brief Record start of a new game
 * @param difficulty Selected difficulty level (1-3)
 * @note Updates difficulty histogram and starts game timer
 */
void svc_stats_game_start(uint32_t difficulty)
{
    game_stats.current_game_start = drv_tiempo_actual_us();
    if (difficulty >= 1 && difficulty <= MAX_DIFFICULTY_LEVEL)
    {
        game_stats.difficulty_histogram[difficulty - 1]++;
    }
    else
    {
        LOG_ERROR_F("Invalid difficulty level: %u", difficulty);
        svc_log_procesar();
    }
}

/**
 * @brief Record end of a game
 * @param sequence_length Length of the sequence played
 * @note Updates sequence stats, duration stats, and histograms
 */
void svc_stats_game_over(uint32_t sequence_length)
{
    game_stats.total_games++;

    // Calculate game duration
    Tiempo_us_t duration = drv_tiempo_actual_us() - game_stats.current_game_start;

    // Update duration stats
    if (duration > game_stats.max_game_duration)
    {
        game_stats.max_game_duration = duration;
    }
    if (duration < game_stats.min_game_duration)
    {
        game_stats.min_game_duration = duration;
    }
    game_stats.avg_game_duration =
        (game_stats.avg_game_duration * (game_stats.total_games - 1) + duration) / game_stats.total_games;

    // Update sequence stats
    if (sequence_length > game_stats.max_sequence)
    {
        game_stats.max_sequence = sequence_length;
    }
    if (sequence_length < game_stats.min_sequence)
    {
        game_stats.min_sequence = sequence_length;
    }

    // Update histogram with bounds checking
    if (sequence_length < MAX_SEQUENCE_LENGTH)
    {
        game_stats.sequences_histogram[sequence_length]++;
    }
    else
    {
        LOG_ERROR_F("Sequence length %u exceeds histogram size", sequence_length);
        svc_log_procesar();
    }

    // Update average
    game_stats.avg_sequence =
        (game_stats.avg_sequence * (game_stats.total_games - 1) + sequence_length) / game_stats.total_games;
}

/******************************************************************************
 * FIFO Statistics
 *****************************************************************************/

/**
 * @brief Start timing a FIFO event
 * @note Called when event is queued
 */
void svc_stats_fifo_start(void)
{
    queue_push(drv_tiempo_actual_us(), &timing_stats.fifo_queue);
    fifo_stats.total_events++;
}

/**
 * @brief End timing a FIFO event
 * @note Called when event is processed, updates timing statistics
 */
void svc_stats_fifo_end(void)
{
    Tiempo_us_t start = queue_pop(&timing_stats.fifo_queue);
    if (start == 0)
        return;

    Tiempo_us_t duration = drv_tiempo_actual_us() - start;

    // Update statistics
    timing_stats.fifo_total_time += duration;
    timing_stats.fifo_avg_time = timing_stats.fifo_total_time / fifo_stats.total_events;

    if (duration > timing_stats.fifo_max_time)
    {
        timing_stats.fifo_max_time = duration;
    }
    if (duration < timing_stats.fifo_min_time)
    {
        timing_stats.fifo_min_time = duration;
    }
}

/******************************************************************************
 * Interrupt Statistics
 *****************************************************************************/

/**
 * @brief Start timing an interrupt
 * @note Called when external interrupt occurs
 */
void svc_stats_interrupt_start(void)
{
    queue_push(drv_tiempo_actual_us(), &timing_stats.interrupt_queue);
    timing_stats.interrupt_count++;
}

/**
 * @brief End timing an interrupt
 * @note Called when interrupt handling completes, updates timing statistics
 */
void svc_stats_interrupt_end(void)
{
    Tiempo_us_t start = queue_pop(&timing_stats.interrupt_queue);
    if (start == 0)
        return;

    Tiempo_us_t duration = drv_tiempo_actual_us() - start;

    timing_stats.interrupt_total_time += duration;
    timing_stats.interrupt_avg_time =
        timing_stats.interrupt_total_time / timing_stats.interrupt_count;

    if (duration > timing_stats.interrupt_max_time)
    {
        timing_stats.interrupt_max_time = duration;
    }
    if (duration < timing_stats.interrupt_min_time)
    {
        timing_stats.interrupt_min_time = duration;
    }
}

/******************************************************************************
 * User Response Statistics
 *****************************************************************************/

/**
 * @brief Start timing user response
 * @note Called when sequence display ends
 */
void svc_stats_user_response_start(void)
{
    queue_push(drv_tiempo_actual_us(), &timing_stats.user_queue);
}

/**
 * @brief End timing user response
 * @note Called when user presses button, updates timing statistics
 */
void svc_stats_user_response_end(void)
{
    Tiempo_us_t start = queue_pop(&timing_stats.user_queue);
    if (start == 0)
        return;

    Tiempo_us_t duration = drv_tiempo_actual_us() - start;

    timing_stats.user_total_time += duration;
    timing_stats.user_response_count++;
    timing_stats.user_avg_time =
        timing_stats.user_total_time / timing_stats.user_response_count;

    if (duration > timing_stats.user_max_time)
    {
        timing_stats.user_max_time = duration;
    }
    if (duration < timing_stats.user_min_time)
    {
        timing_stats.user_min_time = duration;
    }
}

/******************************************************************************
 * Power Management Statistics
 *****************************************************************************/

/**
 * @brief Record wait mode entry
 * @note Updates wait cycle count and starts timing
 */
void svc_stats_wait_enter(void)
{
    queue_push(drv_tiempo_actual_us(), &timing_stats.wait_queue);
    timing_stats.wait_cycles++;
}

/**
 * @brief Record wait mode exit
 * @note Updates wait timing statistics
 */
void svc_stats_wait_exit(void)
{
    Tiempo_us_t start = queue_pop(&timing_stats.wait_queue);
    if (start == 0)
        return;

    Tiempo_us_t duration = drv_tiempo_actual_us() - start;

    timing_stats.wait_total_time += duration;
    timing_stats.wait_avg_time =
        timing_stats.wait_total_time / timing_stats.wait_cycles;

    if (duration > timing_stats.wait_max_time)
    {
        timing_stats.wait_max_time = duration;
    }
    if (duration < timing_stats.wait_min_time)
    {
        timing_stats.wait_min_time = duration;
    }
}

/**
 * @brief Record sleep mode entry
 * @note Updates sleep cycle count and starts timing
 */
void svc_stats_sleep_enter(void)
{
    queue_push(drv_tiempo_actual_us(), &timing_stats.sleep_queue);
    timing_stats.sleep_cycles++;
}

/**
 * @brief Record sleep mode exit
 * @note Updates sleep timing statistics
 */
void svc_stats_sleep_exit(void)
{
    Tiempo_us_t start = queue_pop(&timing_stats.sleep_queue);
    if (start == 0)
        return;

    Tiempo_us_t duration = drv_tiempo_actual_us() - start;

    timing_stats.sleep_total_time += duration;
    timing_stats.sleep_avg_time =
        timing_stats.sleep_total_time / timing_stats.sleep_cycles;

    if (duration > timing_stats.sleep_max_time)
    {
        timing_stats.sleep_max_time = duration;
    }
    if (duration < timing_stats.sleep_min_time)
    {
        timing_stats.sleep_min_time = duration;
    }
}

/******************************************************************************
 * Debug Output Functions
 *****************************************************************************/

/**
 * @brief Dump FIFO queue statistics to the log
 * @note Shows event counts from rt_FIFO_estadisticas, timing, and distribution
 */
void svc_stats_dump_fifo(uint8_t process_them)
{
    if (process_them)
        drv_sc_entrar();

    LOG_INFO("=== FIFO Statistics ===");

    // Get total events from FIFO module
    uint32_t total_events = rt_FIFO_estadisticas(ev_VOID);
    LOG_INFO_F("Total events: %u", total_events);
    LOG_INFO_F("Unhandled events: %u", fifo_stats.unhandled_events);

    // Timing statistics
    LOG_INFO_F("Max wait time: %u us", timing_stats.fifo_max_time);
    LOG_INFO_F("Min wait time: %u us", timing_stats.fifo_min_time == UINT32_MAX ? 0 : timing_stats.fifo_min_time);
    LOG_INFO_F("Avg wait time: %u us", timing_stats.fifo_avg_time);

    // Get event distribution from FIFO module
    LOG_INFO("Events by type:");
    for (int i = 0; i < EVENT_TYPES; i++)
    {
        uint32_t type_count = rt_FIFO_estadisticas((EVENTO_T)i);
        if (type_count > 0)
        {
            LOG_INFO_F("- Type %d: %u (unhandled: %u)",
                       i,
                       type_count,
                       fifo_stats.events_by_type_unhandled[i]);
        }
    }

    if (process_them)
    {
        drv_sc_salir();
        svc_log_procesar();
    }
}

/**
 * @brief Dump timing statistics to the log
 * @note Shows interrupt, user response, wait and sleep timing stats
 */
void svc_stats_dump_timing(uint8_t process_them)
{
    if (process_them)
        drv_sc_entrar();
		
    LOG_INFO("=== Timing Statistics ===");

    LOG_INFO("External interrupt Times:");
    LOG_INFO_F("- Count: %u", timing_stats.interrupt_count);
    LOG_INFO_F("- Total: %u us", timing_stats.interrupt_total_time);
    LOG_INFO_F("- Maximum: %u us", timing_stats.interrupt_max_time);
    LOG_INFO_F("- Minimum: %u us", timing_stats.interrupt_min_time == UINT32_MAX ? 0 : timing_stats.interrupt_min_time);
    LOG_INFO_F("- Average: %u us", timing_stats.interrupt_avg_time);

    LOG_INFO("User Response Times:");
    LOG_INFO_F("- Count: %u", timing_stats.user_response_count);
    LOG_INFO_F("- Total: %u us", timing_stats.user_total_time);
    LOG_INFO_F("- Maximum: %u us", timing_stats.user_max_time);
    LOG_INFO_F("- Minimum: %u us", timing_stats.user_min_time == UINT32_MAX ? 0 : timing_stats.user_min_time);
    LOG_INFO_F("- Average: %u us", timing_stats.user_avg_time);

    LOG_INFO("Wait Mode Times:");
    LOG_INFO_F("- Cycles: %u", timing_stats.wait_cycles);
    LOG_INFO_F("- Total: %u us", timing_stats.wait_total_time);
    LOG_INFO_F("- Maximum: %u us", timing_stats.wait_max_time);
    LOG_INFO_F("- Minimum: %u us", timing_stats.wait_min_time == UINT32_MAX ? 0 : timing_stats.wait_min_time);
    LOG_INFO_F("- Average: %u us", timing_stats.wait_avg_time);

    LOG_INFO("Sleep Mode Times:");
    LOG_INFO_F("- Cycles: %u", timing_stats.sleep_cycles);
    LOG_INFO_F("- Total: %u us", timing_stats.sleep_total_time);
    LOG_INFO_F("- Maximum: %u us", timing_stats.sleep_max_time);
    LOG_INFO_F("- Minimum: %u us", timing_stats.sleep_min_time == UINT32_MAX ? 0 : timing_stats.sleep_min_time);
    LOG_INFO_F("- Average: %u us", timing_stats.sleep_avg_time);

    if (process_them)
    {
        drv_sc_salir();
        svc_log_procesar();
    }
}

/**
 * @brief Dump power management statistics to the log
 * @note Shows active/sleep times and cycle counts
 */
void svc_stats_dump_power(uint8_t process_them)
{
    if (process_them)
        drv_sc_entrar();

    LOG_INFO("=== Power Statistics ===");
    LOG_INFO_F("Active time: %u us", power_stats.total_active_time);
    LOG_INFO_F("Sleep time: %u us", power_stats.total_sleep_time);
    LOG_INFO_F("Sleep cycles: %u", power_stats.sleep_cycles);
    LOG_INFO_F("Wake-ups: %u", power_stats.wakeups);

    if (process_them)
    {
        drv_sc_salir();
        svc_log_procesar();
    }
}

/**
 * @brief Dump game performance statistics to the log
 * @note Shows sequence lengths, difficulty distribution, and timing
 */
void svc_stats_dump_game(uint8_t process_them)
{
    if (process_them)
        drv_sc_entrar();

    LOG_INFO("=== Game Statistics ===");
    LOG_INFO_F("Total games: %u", game_stats.total_games);
    LOG_INFO_F("Best sequence: %u", game_stats.max_sequence);
    LOG_INFO_F("Worst sequence: %u", game_stats.min_sequence == UINT32_MAX ? 0 : game_stats.min_sequence);
    LOG_INFO_F("Average sequence: %.2f", game_stats.avg_sequence);
    LOG_INFO_F("Longest game: %u us", game_stats.max_game_duration);
    LOG_INFO_F("Shortest game: %u us", game_stats.min_game_duration == UINT32_MAX ? 0 : game_stats.min_game_duration);
    LOG_INFO_F("Average duration: %u us", game_stats.avg_game_duration);

    LOG_INFO("Sequence length distribution:");
    for (int i = 0; i < MAX_SEQUENCE_LENGTH; i++)
    {
        if (game_stats.sequences_histogram[i] > 0)
        {
            LOG_INFO_F("- Length %d: %u games", i, game_stats.sequences_histogram[i]);
        }
    }

    LOG_INFO("Difficulty distribution:");
    for (int i = 0; i < MAX_DIFFICULTY_LEVEL; i++)
    {
        LOG_INFO_F("- Difficulty %d: %u games", i + 1, game_stats.difficulty_histogram[i]);
    }

    if (process_them)
    {
        drv_sc_salir();
        svc_log_procesar();
    }
}

/**
 * @brief Dump all statistics to the log
 * @note Calls all individual dump functions in sequence
 */
void svc_stats_dump_all(uint8_t process_them)
{
    if (process_them)
        drv_sc_entrar();
		
    LOG_INFO("=== STATISTICS SUMMARY ===\n");
    svc_stats_dump_fifo(process_them);
    svc_stats_dump_timing(process_them);
    svc_stats_dump_power(process_them);
    svc_stats_dump_game(process_them);
    LOG_INFO("=== END OF STATISTICS SUMMARY ===\n");

    if (process_them)
    {
        drv_sc_salir();
        svc_log_procesar();
    }
}

#endif
