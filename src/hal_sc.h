/**
 * @file hal_sc.h
 * @ingroup HAL
 * @brief Critical Section HAL Interface
 * @details Interface for the Critical Section Hardware Abstraction Layer.
 *          Provides functions to manage interrupt enabling/disabling.
 *
 * @defgroup HAL_SC Critical Section HAL
 * @ingroup HAL
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

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
