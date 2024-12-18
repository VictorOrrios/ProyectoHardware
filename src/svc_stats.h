/**
 * @file svc_stats.h
 * @ingroup SVC_STATS
 * @brief Statistics Service Interface
 * @details Interface for the statistics service module. Provides functions and data
 *          structures to collect and analyze system performance metrics including:
 *          - FIFO queue statistics (events, wait times)
 *          - Response time measurements (IRQ and user)
 *          - Power management tracking (sleep/wake cycles)
 *          - Game performance metrics (sequence lengths, difficulty)
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#ifndef SVC_STATS
#define SVC_STATS

#include <stdint.h>

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
