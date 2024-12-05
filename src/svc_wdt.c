/* *****************************************************************************
 * Hardware Project 2024
 * 
 * svc_wdt.c - Watchdog Service Implementation
 * 
 * Description:
 *   Implements the watchdog service defined in svc_wdt.h. Monitors registered
 *   tasks for activity and feeds the watchdog timer if all tasks report within
 *   the specified timeout. Supports callback registration for watchdog events.
 * 
 * Implementation Notes:
 *   - No critical sections needed as all functions are called in thread mode
 *   - Uses periodic checks to ensure tasks are alive
 *   - Handles task registration and activity reporting
 * *****************************************************************************/

#include "svc_wdt.h"
#include "drv_wdt.h"
#include "svc_alarma.h"
#include "drv_monitor.h"
#include "drv_tiempo.h"
#include "rt_GE.h"
#include "svc_log.h"
#include <string.h>

#define MAX_TASKS 4
#define CHECK_PERIOD_FACTOR 4  // Check 4 times within timeout period

/*
 * NOTE: Technically, in this watchdog module, critical sections are not needed,
 * because all the functions defined here are called in thread mode (those
 * programmed as callbacks subscribed to events as well, since the event manager
 * will call them, and it will do so in thread mode). Therefore, with only one
 * execution thread, there cannot be race conditions on the shared structures.
 */

typedef struct
{
    uint32_t task_id;
    uint32_t last_check;
    uint8_t active;
} Task_WDT;

static Task_WDT tasks[MAX_TASKS];
static uint32_t monitor_id;
static uint32_t check_period_ms;  // Calculated from timeout

/**
 * @brief Callback for periodic task checks
 * @param evento Event type (unused)
 * @param aux Additional data (unused)
 */
static void check_tasks(uint32_t evento, uint32_t aux)
{
    uint32_t current_time = drv_tiempo_actual_ms();
    uint8_t all_tasks_ok = 1;

    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (tasks[i].active)
        {
            // Check if task has reported within timeout
            if (current_time - tasks[i].last_check > check_period_ms)
            {
                all_tasks_ok = 0;
                break;
            }
        }
    }

    if (all_tasks_ok)
    {
        drv_wdt_feed();
    }
}

/**
 * @brief Initialize the watchdog service
 * @param timeout_ms Timeout in milliseconds
 * @param monitor Monitor ID for overflow detection
 */
void svc_wdt_iniciar(uint32_t timeout_ms, uint32_t monitor)
{
    monitor_id = monitor;

    // Calculate check period (timeout/4 to have margin)
    check_period_ms = timeout_ms / CHECK_PERIOD_FACTOR;

    // Initialize task array
    memset(tasks, 0, sizeof(tasks));

    // Initialize driver
    drv_wdt_iniciar(timeout_ms, monitor);

    // Setup periodic check using alarm service
    svc_alarma_activar(svc_alarma_codificar(1, check_period_ms), ev_WDT_CHECK, 0);
    svc_GE_suscribir(ev_WDT_CHECK, check_tasks);
}

/**
 * @brief Register a task for watchdog monitoring
 * @param task_id Task identifier
 * @return Task handle or -1 if error
 */
int32_t svc_wdt_registrar_tarea(uint32_t task_id)
{
    // Find free slot
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (!tasks[i].active)
        {
            tasks[i].task_id = task_id;
            tasks[i].last_check = drv_tiempo_actual_ms();
            tasks[i].active = 1;
            return i;
        }
    }

    // No free slots found - mark monitor and return error
    drv_monitor_marcar(monitor_id);
		LOG_ERROR_F("No free slot while trying to add task %d",task_id);
		svc_log_procesar();
    while (1);
    // return -1;
}

/**
 * @brief Report that a task is alive
 * @param handle Handle returned by svc_wdt_registrar_tarea
 */
void svc_wdt_tarea_viva(uint32_t handle)
{
    if (handle < MAX_TASKS && tasks[handle].active)
    {
        tasks[handle].last_check = drv_tiempo_actual_ms();
    }
}

/**
 * @brief Register a callback for watchdog warning interrupt
 * @param callback Function to call when watchdog is about to reset
 */
void svc_wdt_registrar_callback(void (*callback)(void))
{
    drv_wdt_registrar_callback(callback);
}
