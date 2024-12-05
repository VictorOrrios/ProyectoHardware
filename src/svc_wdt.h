/* *****************************************************************************
 * Hardware Project 2024
 * 
 * svc_wdt.h - Watchdog Service Interface
 * 
 * Description:
 *   Provides an interface for the watchdog service, allowing tasks to be
 *   monitored for activity. Supports task registration, periodic checks,
 *   and callback registration for watchdog events.
 * 
 * Key Features:
 *   - Task registration and monitoring
 *   - Periodic task activity checks
 *   - Watchdog feeding and callback support
 * *****************************************************************************/

#ifndef SVC_WDT
#define SVC_WDT

#include <stdint.h>

/**
 * @brief Initialize watchdog service
 * @param timeout_ms Timeout in milliseconds
 * @param monitor Monitor for overflow/errors
 */
void svc_wdt_iniciar(uint32_t timeout_ms, uint32_t monitor);

/**
 * @brief Register task for watchdog monitoring
 * @param task_id Task identifier
 * @return Task handle or -1 if error
 */
int32_t svc_wdt_registrar_tarea(uint32_t task_id);

/**
 * @brief Report task is alive
 * @param handle Handle returned by svc_wdt_registrar_tarea
 */
void svc_wdt_tarea_viva(uint32_t handle);

/**
 * @brief Register a callback for watchdog warning interrupt
 * @param callback Function to call when watchdog is about to reset
 */
void svc_wdt_registrar_callback(void (*callback)(void));

#endif
