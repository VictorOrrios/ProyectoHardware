/**
 * @file hal_consumo_lpc.c
 * @ingroup HAL_LPC
 * @brief Power Management HAL implementation for LPC2105
 * @details Implementation of the Power Management Hardware Abstraction Layer for
 *          the LPC2105 microcontroller. This module provides functions to manage
 *          different power modes (idle and sleep) to optimize power consumption.
 *
 * @defgroup HAL_LPC_PM Power Management
 * @ingroup HAL_LPC
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "hal_consumo.h"
#include "board.h"
#include <LPC210x.H>
#include <stdint.h>

extern void Switch_to_PLL(void);

/**
 * @brief Initialize the power management system
 * 
 * Configures the necessary registers to enable different
 * low power modes of the microcontroller.
 */
void hal_consumo_iniciar(void) {}

/**
 * @brief Put the microcontroller in wait mode
 * 
 * In this mode, the processor stops execution but keeps peripherals active.
 * It wakes up on any interrupt. Moderate power consumption.
 */
void hal_consumo_esperar(void) {
    EXTWAKE |= 0b111;  // NOTE: Reassign value as it might have been written from elsewhere
    PCON = 0b1;        // Put microcontroller in IDLE mode
}

/**
 * @brief Put the microcontroller in sleep mode
 * 
 * Minimum power consumption mode where most peripherals and the main clock
 * are stopped. Only essential peripherals configured to wake up the system
 * remain active.
 */
void hal_consumo_dormir(void) {
    EXTWAKE |= 0b111;  // Reassign value as it might have been written from elsewhere
    PCON |= 0b10;      // Enter sleep mode
    Switch_to_PLL();   // Restore clock on wake-up
}

/** @} */ // End of HAL_LPC_PM group
