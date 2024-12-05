/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_sc.h - Critical Section HAL Interface
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
 *   Interface for the Critical Section Hardware Abstraction Layer.
 *   Provides functions to manage interrupt enabling/disabling for
 *   critical section implementation.
 * *****************************************************************************/

#ifndef HAL_SC
#define HAL_SC

#include <stdint.h>

/**
 * @brief Initialize critical section hardware resources
 */
void hal_sc_iniciar(void);

/**
 * @brief Enter critical section by disabling interrupts
 * 
 * @return uint32_t Previous interrupt state
 */
uint32_t hal_sc_entrar(void);

/**
 * @brief Exit critical section and restore interrupts
 * 
 * @param previous_state Previous interrupt state to restore
 */
void hal_sc_salir(uint32_t previous_state);

#endif // HAL_SC
