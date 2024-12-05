/* *****************************************************************************
 * Hardware Project 2024
 * 
 * svc_stats.h - Statistics Service Interface
 * 
 * Authors:
 *   - Víctor Orrios Barón (NIA: 840994)
 *   - José Miguel Quílez Vergara (NIA: 873499)
 * 
 * EINA - University of Zaragoza
 * Computer Science and Engineering
 * Course: 3rd year, 1st semester
 * 
 * Date: 02/12/2024
 * 
 * Description:
 *   Interface for the statistics service module. Provides functions and data
 *   structures to collect and analyze system performance metrics including:
 *   - FIFO queue statistics (events, wait times)
 *   - Response time measurements (IRQ and user)
 *   - Power management tracking (sleep/wake cycles)
 *   - Game performance metrics (sequence lengths, difficulty)
 * 
 * Design Notes:
 *   - Uses circular buffers for timing measurements
 *   - All timing values are in microseconds (us)
 *   - Histograms track distribution of game metrics
 *   - Thread-safe timing queue implementation
 *   - Minimal memory footprint with fixed-size arrays
 * 
 * Dependencies:
 *   - stdint.h: Standard integer types
 *   - drv_tiempo.h: Time measurement functions
 *   - rt_evento_t.h: Event type definitions
 * 
 * Usage:
 *   1. Initialize module with svc_stats_iniciar()
 *   2. Call appropriate timing functions at event boundaries
 *   3. Use dump functions to output collected statistics
 * *****************************************************************************/

#ifndef SVC_STATS
#define SVC_STATS

#include <stdint.h>
#include "drv_tiempo.h"
#include "rt_evento_t.h"

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

#ifdef DEBUG_STATS

/**
 * @brief Initialize the statistics module
 * Sets all counters and timers to initial values
 * Initializes min values to maximum possible value
 */
void svc_stats_iniciar(void);

/**
 * @brief Dump all statistics to the log
 * Calls all individual dump functions in sequence
 */
void svc_stats_dump_all(uint8_t process_them);

/**
 * @brief Dump FIFO queue statistics to the log
 * Shows event counts, timing, and distribution by type
 * Uses rt_FIFO_estadisticas for accurate event counts
 */
void svc_stats_dump_fifo(uint8_t process_them);

/**
 * @brief Dump timing statistics to the log
 * Shows interrupt, user response, wait and sleep timing stats
 */
void svc_stats_dump_timing(uint8_t process_them);

/**
 * @brief Dump power management statistics to the log
 * Shows active/sleep times and cycle counts
 */
void svc_stats_dump_power(uint8_t process_them);

/**
 * @brief Dump game performance statistics to the log
 * Shows sequence lengths, difficulty distribution, and timing
 */
void svc_stats_dump_game(uint8_t process_them);

// Add new function declarations:
/**
 * @brief Start timing an interrupt
 * Called when external interrupt occurs
 */
void svc_stats_interrupt_start(void);

/**
 * @brief End timing an interrupt
 * Called when interrupt handling completes
 */
void svc_stats_interrupt_end(void);

/**
 * @brief Record wait mode entry
 */
void svc_stats_wait_enter(void);

/**
 * @brief Record wait mode exit
 */
void svc_stats_wait_exit(void);

/**
 * @brief Record sleep mode entry
 */
void svc_stats_sleep_enter(void);

/**
 * @brief Record sleep mode exit
 */
void svc_stats_sleep_exit(void);

/**
 * @brief Start timing a FIFO event
 * Called when event is queued
 */
void svc_stats_fifo_start(void);

/**
 * @brief End timing a FIFO event
 * Called when event is processed
 * @return Tiempo_us_t Time taken to process event
 */
void svc_stats_fifo_end(void);

/**
 * @brief Start timing user response
 * Called when sequence display ends
 */
void svc_stats_user_response_start(void);

/**
 * @brief End timing user response
 * Called when user presses button
 * @return Tiempo_us_t Time taken for user to respond
 */
void svc_stats_user_response_end(void);

/********************************************************
 * Game Stats
 ********************************************************/

/**
 * @brief Record start of a new game
 * @param difficulty Selected difficulty level (1-3)
 */
void svc_stats_game_start(uint32_t difficulty);

/**
 * @brief Record end of a game
 * @param sequence_length Length of the sequence played
 */
void svc_stats_game_over(uint32_t sequence_length);

#else
    #define svc_stats_iniciar()               ((void)0)
    #define svc_stats_dump_all(p)              ((void)0)
    #define svc_stats_dump_fifo(p)             ((void)0)
    #define svc_stats_dump_timing(p)           ((void)0)
    #define svc_stats_dump_power(p)            ((void)0)
    #define svc_stats_dump_game(p)             ((void)0)
    #define svc_stats_interrupt_start()       ((void)0)
    #define svc_stats_interrupt_end()         ((void)0)
    #define svc_stats_wait_enter()            ((void)0)
    #define svc_stats_wait_exit()             ((void)0)
    #define svc_stats_sleep_enter()           ((void)0)
    #define svc_stats_sleep_exit()            ((void)0)
    #define svc_stats_fifo_start()            ((void)0)
    #define svc_stats_fifo_end()              ((void)0)
    #define svc_stats_user_response_start()   ((void)0)
    #define svc_stats_user_response_end()     ((void)0)
    #define svc_stats_game_start(d)           ((void)0)
    #define svc_stats_game_over(s)            ((void)0)
#endif // DEBUG_STATS

#endif // SVC_STATS
