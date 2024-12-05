/* *****************************************************************************
 * Hardware Project 2024
 * 
 * drv_wdt.c - Watchdog Timer Driver Implementation
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
 *   Implementation of the Watchdog Timer driver module. Provides watchdog
 *   management with debug monitoring capabilities through monitor signals.
 * *****************************************************************************/

#include "drv_wdt.h"
#include "hal_wdt.h"
#include "drv_monitor.h"

/** Global monitor ID for watchdog operations */
static uint32_t monitor_id_global;

/**
 * @brief Initialize watchdog driver
 * 
 * @param timeout_ms Timeout period in milliseconds
 * @param monitor Monitor ID for debugging
 */
void drv_wdt_iniciar(uint32_t timeout_ms, uint32_t monitor) {
    monitor_id_global = monitor;
    
    // Unmark monitor before initialization
    drv_monitor_desmarcar(monitor_id_global);
    
    // Initialize hardware watchdog
    hal_wdt_iniciar(timeout_ms);
    
    // Mark monitor after initialization
    drv_monitor_marcar(monitor_id_global);
}

/**
 * @brief Feed/kick the watchdog
 * 
 * Prevents watchdog from triggering by feeding it within timeout period.
 * Uses monitor to track feeding operations.
 * 
 * @note Marks monitor before and after feeding
 */
void drv_wdt_feed(void) {
    // Unmark monitor before feeding
    drv_monitor_desmarcar(monitor_id_global);
    
    // Feed hardware watchdog
    hal_wdt_feed();
    
    // Mark monitor after feeding
    drv_monitor_marcar(monitor_id_global);
}

/**
 * @brief Register callback for watchdog events
 * 
 * @param callback Function to call when watchdog triggers
 */
void drv_wdt_registrar_callback(void (*callback)(void)) {
		hal_wdt_registrar_callback(callback);
}
