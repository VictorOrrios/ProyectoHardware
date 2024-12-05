/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_sc_nrf.c - Critical Section HAL for nRF52840
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
 *   the nRF52840 microcontroller. Uses PRIMASK register for interrupt control,
 *   supporting nested critical sections by preserving the previous state.
 * *****************************************************************************/

#include "hal_sc.h"
#include "nrf.h"

#ifdef DEBUG
#include <assert.h>
#endif

/**
 * @brief Initialize the critical section manager
 */
void hal_sc_iniciar(void) {
    // __enable_irq();
    
    #ifdef DEBUG
    assert(__get_PRIMASK() == 0);
    #endif
}

/**
 * @brief Enter a critical section
 * 
 * @return uint32_t Previous PRIMASK value (0=enabled, 1=disabled)
 */
uint32_t hal_sc_entrar(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    
    #ifdef DEBUG
    assert(__get_PRIMASK() == 1);
    #endif
    
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
