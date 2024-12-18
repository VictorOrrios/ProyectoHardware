/**
 * @file hal_ext_int.h
 * @ingroup HAL
 * @brief External Interrupts HAL Interface
 * @details Interface for the External Interrupts Hardware Abstraction Layer.
 *          Provides functions to manage external interrupts and wake-up
 *          capabilities.
 *
 * @defgroup HAL_EXTINT External Interrupts HAL
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

#ifndef HAL_EXT_INT
#define HAL_EXT_INT

#include <stdint.h>

/**
 * @brief Initialize external interrupts system
 * 
 * Configures hardware required to manage external interrupts
 * and wake-up capabilities from low power mode.
 */
void hal_ext_int_iniciar(void);

/**
 * @brief Enable external interrupts for a pin
 * 
 * @param pin Pin number to enable interrupts for
 */
void hal_ext_int_habilitar_int(uint32_t pin);

/**
 * @brief Disable external interrupts for a pin
 * 
 * @param pin Pin number to disable interrupts for
 */
void hal_ext_int_deshabilitar_int(uint32_t pin);

/**
 * @brief Enable wake-up capability for a pin
 * 
 * @param pin Pin number that will be able to wake up the system
 */
void hal_ext_int_habilitar_despertar(uint32_t pin);

/**
 * @brief Disable wake-up capability for a pin
 * 
 * @param pin Pin number that will no longer wake up the system
 */
void hal_ext_int_deshabilitar_despertar(uint32_t pin);

/**
 * @brief Register callback function for external interrupts
 * 
 * Registers a handler that will be called when an external interrupt
 * occurs. The callback receives the pin number that generated the
 * interrupt as a parameter.
 * 
 * @param callback Function to call on external interrupt.
 *                 If NULL, notifications will be disabled.
 */
void hal_ext_int_registrar_callback(void (*callback)(uint32_t));

/**
 * @brief Get the current state of a gpio pin
 * 
 * @param pin Pin number to check
 * @return uint8_t Current state of the pin
 */
uint8_t hal_ext_int_get_estado_pin(uint32_t pin);

#endif // HAL_EXT_INT

