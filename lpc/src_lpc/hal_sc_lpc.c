/**
 * @file hal_sc_lpc.c
 * @ingroup HAL_LPC
 * @brief Critical Section HAL implementation for LPC2105
 * @details Implementation of the Critical Section Hardware Abstraction Layer for
 *          the LPC2105 microcontroller. This module manages interrupt enabling
 *          and disabling for critical sections using the Vectored Interrupt
 *          Controller (VIC).
 *
 * @defgroup HAL_LPC_SC Critical Sections
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

/** @} */ // End of HAL_LPC_SC group
