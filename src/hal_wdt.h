/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_wdt.h - Watchdog Timer HAL Interface
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
 *   Interface for the Watchdog Timer Hardware Abstraction Layer. Provides
 *   functions to configure and manage the hardware watchdog timer for
 *   system supervision and recovery from software failures.
 * *****************************************************************************/

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
