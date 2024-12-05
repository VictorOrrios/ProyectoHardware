/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_sc_lpc.c - Critical Section HAL for LPC2105
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
 *   Implementation of the Critical Section Hardware Abstraction Layer for
 *   the LPC2105 microcontroller. This module manages interrupt enabling
 *   and disabling for critical sections using the Vectored Interrupt
 *   Controller (VIC).
 * *****************************************************************************/

#include "hal_sc.h"
#include <LPC210x.H>

/**
 * @brief Initialize the critical section manager
 */
void hal_sc_iniciar(void) {}

/**
 * @brief Enter a critical section
 * 
 * @return uint32_t Previous VICIntEnable state
 */
uint32_t hal_sc_entrar(void) {
    // Must save state before clearing
    uint32_t previous_state = VICIntEnable;
    VICIntEnClr = 0xFFFFFFFF;
    return previous_state;
}

/**
 * @brief Exit a critical section
 * 
 * @param previous_state Previous VICIntEnable value from hal_sc_entrar()
 */
void hal_sc_salir(uint32_t previous_state) {
    // Directly restore the previous enable state
    VICIntEnable = previous_state;
}
