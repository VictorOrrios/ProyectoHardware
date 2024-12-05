/* *****************************************************************************
 * Hardware Project 2024
 * 
 * drv_consumo.c - Power Management Driver
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
 *   Implementation of the power management driver. This module manages different
 *   power consumption modes of the microcontroller and monitors state changes
 *   using monitor signals.
 * *****************************************************************************/
  
#include "drv_consumo.h"
#include "drv_monitor.h"
#include "hal_consumo.h"
#include "svc_stats.h"

/** Global variables to store wait and sleep mode monitors */
static volatile uint32_t mon_wait_global, mon_dormir_global;

/**
 * @brief Initialize the power management driver
 * 
 * @param mon_wait Monitor for wait mode
 * @param mon_dormir Monitor for sleep mode
 * 
 * @pre hal_consumo and drv_monitor modules must be available
 * @note Configures global monitors and initializes required subsystems
 */
void drv_consumo_iniciar(uint32_t mon_wait, uint32_t mon_dormir){
    mon_wait_global = mon_wait;
    mon_dormir_global = mon_dormir;
    drv_monitor_marcar(mon_wait_global);
    drv_monitor_marcar(mon_dormir_global);
    hal_consumo_iniciar();
}

/**
 * @brief Put the system in wait mode
 * 
 * Unmarks the wait monitor, enters low power mode,
 * and marks the monitor again upon wake-up.
 * 
 * @pre Driver must be initialized
 * @note Uses the monitor configured during initialization
 */
void drv_consumo_esperar(void){
    svc_stats_wait_enter();
    drv_monitor_desmarcar(mon_wait_global);
    hal_consumo_esperar();
    drv_monitor_marcar(mon_wait_global);
    svc_stats_wait_exit();
}

/**
 * @brief Put the system in sleep mode
 * 
 * Unmarks the sleep monitor, enters deep low power mode,
 * and marks the monitor again upon wake-up.
 * 
 * @pre Driver must be initialized
 * @note Uses the monitor configured during initialization
 */
void drv_consumo_dormir(void){
    svc_stats_sleep_enter();
    drv_monitor_desmarcar(mon_dormir_global);
    hal_consumo_dormir();
    drv_monitor_marcar(mon_dormir_global);
    svc_stats_sleep_exit();
}
