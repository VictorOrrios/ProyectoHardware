/**
 * @file drv_monitor.h
 * @ingroup DRV
 * @brief Debug Monitor Driver Interface
 * @details Interface for the debug monitor driver module. Defines functions to
 *          manage GPIO pins used as debug monitors, allowing temporal tracking
 *          of program execution through digital signals.
 *
 * @defgroup DRV_MONITOR Debug Monitor Driver
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

#ifndef DRV_MONITOR
#define DRV_MONITOR

#include <stdint.h>

/**
 * @brief Initialize debug monitors
 * 
 * Configures GPIO pins as outputs and sets their initial state.
 * 
 * @return uint32_t Number of available monitors on the platform
 * @pre MONITOR_NUMBER and MONITOR_LIST must be defined in board.h
 */
uint32_t drv_monitor_iniciar(void);

/**
 * @brief Activate (mark) a specific monitor
 * 
 * @param id Monitor identifier (1 to MONITOR_NUMBER)
 * @pre Monitor must be initialized
 * @note Active state is defined by MONITOR_ACTIVE_STATE
 */
void drv_monitor_marcar(uint32_t id);

/**
 * @brief Deactivate (unmark) a specific monitor
 * 
 * @param id Monitor identifier (1 to MONITOR_NUMBER)
 * @pre Monitor must be initialized
 * @note Inactive state is the complement of MONITOR_ACTIVE_STATE
 */
void drv_monitor_desmarcar(uint32_t id);

/** @} */ // End of DRV_MONITOR group

#endif
