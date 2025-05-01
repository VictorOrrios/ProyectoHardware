/**
 * @file drv_wdt.h
 * @ingroup DRV
 * @brief Watchdog Timer Driver Interface
 * @details Interface for the Watchdog Timer driver module. Provides functions
 *          to manage system watchdog with debug monitoring capabilities.
 *
 * @defgroup DRV_WDT Watchdog Timer Driver
 * @ingroup DRV
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

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

/** @} */ // End of DRV_WDT group

#endif
