/**
 * @file hal_wdt.h
 * @ingroup HAL
 * @brief Watchdog Timer HAL Interface
 * @details Interface for the Watchdog Timer Hardware Abstraction Layer.
 *          Provides functions to configure and manage the hardware
 *          watchdog timer for system supervision.
 *
 * @defgroup HAL_WDT Watchdog Timer HAL
 * @ingroup HAL
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#ifndef HAL_WDT
#define HAL_WDT

#include <stdint.h>

/**
 * @brief Initialize watchdog timer
 * @param timeout_ms Timeout in milliseconds
 */
void hal_wdt_iniciar(uint32_t timeout_ms);

/**
 * @brief Feed/kick the watchdog to prevent reset
 */
void hal_wdt_feed(void);

/**
 * @brief Register callback for WDT warning interrupt
 * @param callback Function to call when WDT is about to reset
 */
void hal_wdt_registrar_callback(void (*callback)(void));

#endif
