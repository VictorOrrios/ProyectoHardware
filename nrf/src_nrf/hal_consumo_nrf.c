/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_consumo_nrf.c - Power Management HAL for nRF52840
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
 *   Implementation of the Power Management Hardware Abstraction Layer for
 *   the nRF52840 microcontroller. This module provides functions to manage
 *   different power modes (wait and sleep) to optimize power consumption.
 * *****************************************************************************/

#include "hal_consumo.h"
#include "board.h"
#include "nrf.h"
#include <stdint.h>

/**
 * @brief Initialize the power management system
 * 
 * Configure the necessary registers to enable different
 * low power modes of the microcontroller.
 */
void hal_consumo_iniciar(void){}

/**
 * @brief Put the microcontroller in wait mode
 * 
 * In this mode, the processor stops execution but keeps peripherals active.
 * It wakes up on any interrupt. Moderate power consumption.
 */
void hal_consumo_esperar(void){
	__wfi();  // Wait for Event
}

/**
 * @brief Put the microcontroller in sleep mode
 * 
 * Minimum power consumption mode where most peripherals and the main clock
 * are stopped. Only essential peripherals configured to wake up the system
 * remain active.
 */
void hal_consumo_dormir(void){
	// Enter System OFF mode (lowest power consumption)
	// 	> Will only wake-up from a reset or power cycle, a wake-up pin configured
	//	  as such or a watchdog reset
	NRF_POWER->SYSTEMOFF = POWER_SYSTEMOFF_SYSTEMOFF_Enter;
	
	// Wait for event to ensure the system enters sleep
	// Note: This code will only execute if something prevents entering System OFF
	while (1) {
		__wfe();
	}
}
