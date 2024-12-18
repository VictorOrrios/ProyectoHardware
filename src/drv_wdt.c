/**
 * @file drv_wdt.c
 * @ingroup DRV_WDT
 * @brief Watchdog Timer Driver Implementation
 * @details Implementation of watchdog timer management with monitor tracking
 *          and periodic feeding through alarms.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "drv_wdt.h"
#include "hal_wdt.h"
#include "drv_monitor.h"
#include "svc_alarma.h"
#include "rt_GE.h"

/** @brief Factor to determine watchdog check frequency */
#define CHECK_PERIOD_FACTOR 4  // Check 4 times within timeout period

/** @brief Global monitor ID for watchdog operations */
static uint32_t monitor_id_global;

/**
 * @brief Redirects to drv_wdt_feed
 * @param evento Event type
 * @param aux Additional event data
 */
void feed_periodic(uint32_t evento, uint32_t aux){
		drv_wdt_feed();
}

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
	
		// Subscribe to the periodic feed event
		svc_GE_suscribir(ev_WDT,feed_periodic);
	
		// Make periodic timing feed alarm based on the period factor
		svc_alarma_activar(svc_alarma_codificar(1,timeout_ms/CHECK_PERIOD_FACTOR),ev_WDT,0);
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
