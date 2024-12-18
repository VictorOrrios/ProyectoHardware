/**
 * @file drv_consumo.c
 * @ingroup DRV_POWER
 * @brief Power Management Driver Implementation
 * @details Implementation of power mode transitions and monitor signal management.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */
  
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
