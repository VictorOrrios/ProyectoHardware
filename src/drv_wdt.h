/* *****************************************************************************
 * Hardware Project 2024
 * 
 * drv_wdt.h - Watchdog Timer Driver Interface
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
 *   Interface for the Watchdog Timer driver module. Provides functions
 *   to manage system watchdog with debug monitoring capabilities.
 * *****************************************************************************/

#ifndef DRV_WDT
#define DRV_WDT

#include <stdint.h>

/**
 * @brief Initialize watchdog driver
 * 
 * @param timeout_ms Timeout period in milliseconds
 * @param monitor Monitor ID for debugging
 */
void drv_wdt_iniciar(uint32_t timeout_ms, uint32_t monitor);

/**
 * @brief Feed/kick the watchdog
 * 
 * Prevents watchdog from triggering by feeding it within timeout period.
 * Uses monitor to track feeding operations.
 * 
 * @note Marks monitor before and after feeding
 */
void drv_wdt_feed(void);

/**
 * @brief Register callback for watchdog events
 * 
 * @param callback Function to call when watchdog triggers
 */
void drv_wdt_registrar_callback(void (*callback)(void));

#endif
