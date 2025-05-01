/**
 * @file hal_sc_nrf.c
 * @ingroup HAL_NRF
 * @brief Critical Section HAL implementation for nRF52840
 * @details Implementation of the Critical Section Hardware Abstraction Layer for
 *          the nRF52840 microcontroller. Uses PRIMASK register for interrupt control,
 *          supporting nested critical sections by preserving the previous state.
 *
 * @defgroup HAL_NRF_SC Critical Sections
 * @ingroup HAL_NRF
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
#include "nrf.h"

/**
 * @brief Initialize the critical section manager
 */
void hal_sc_iniciar(void) {}

/**
 * @brief Enter a critical section
 * 
 * @return uint32_t Previous PRIMASK value (0=enabled, 1=disabled)
 */
uint32_t hal_sc_entrar(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    
    return primask;
}

/**
 * @brief Exit a critical section
 * 
 * @param previous_state Previous PRIMASK value from hal_sc_entrar()
 */
void hal_sc_salir(uint32_t previous_state) {
    __set_PRIMASK(previous_state);
}

/** @} */ // End of HAL_NRF_SC group
