/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_ext_int_nrf.c - External Interrupts HAL for nRF52840
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
 *   Implementation of the External Interrupts Hardware Abstraction Layer for
 *   the nRF52840 microcontroller. This module manages external interrupt
 *   configuration and handling using the GPIOTE peripheral, including wake-up
 *   functionality from low power modes.
 * *****************************************************************************/

#include "hal_ext_int.h"
#include "board.h"
#include "nrf.h"
#include <stdint.h>

#ifndef GPIOTE_CH_NUM
    #define GPIOTE_CH_NUM 8
#endif

static void (*ext_int_callback)(uint32_t pin) = 0;

/**
 * @brief Register a callback function for external interrupts
 * 
 * This function allows registering a handler that will be called when an
 * external interrupt occurs. The callback will receive the pin number
 * that generated the interrupt as a parameter.
 * 
 * @param callback Function to call when an external interrupt occurs.
 *                 If NULL, notifications will be disabled.
 */
void hal_ext_int_registrar_callback(void (*callback)(uint32_t)) {
    ext_int_callback = callback;
}

/**
 * @brief Initialize the external interrupt system
 */
void hal_ext_int_iniciar(void) {
    // Clear any pending GPIOTE interrupts
    NRF_GPIOTE->INTENCLR = 0xFFFFFFFF;
    
    // Clear any pending events in all channels
    for (int i = 0; i < GPIOTE_CH_NUM; i++) {
        NRF_GPIOTE->EVENTS_IN[i] = 0;
    }
    
    // Reset all GPIOTE channel configurations
    for (int i = 0; i < GPIOTE_CH_NUM; i++) {
        NRF_GPIOTE->CONFIG[i] = 0;
    }
    
    // Set GPIOTE interrupt priority (optional)
    NVIC_SetPriority(GPIOTE_IRQn, 2); // Medium priority (0 is highest)
    
    // Enable GPIOTE interrupt in NVIC
    NVIC_EnableIRQ(GPIOTE_IRQn);
    
    // Initialize callback to 0
    ext_int_callback = 0;
}

/**
 * @brief Enable external interrupts for a specific pin
 */
void hal_ext_int_habilitar_int(uint32_t pin) {
    // Find a free GPIOTE channel
    for (int i = 0; i < GPIOTE_CH_NUM; i++) {
        if ((NRF_GPIOTE->CONFIG[i] & GPIOTE_CONFIG_MODE_Msk) == 0) {
            // Configure GPIOTE event for the pin => will only interrupt on falling edge
            NRF_GPIOTE->CONFIG[i] = 
                (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
                (pin << GPIOTE_CONFIG_PSEL_Pos) |
                (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos);
            
            // Clear any pending event
            NRF_GPIOTE->EVENTS_IN[i] = 0;
            
            // Enable interrupt for this event
            NRF_GPIOTE->INTENSET = (1 << i);
            break;
        }
    }
}

/**
 * @brief Disable external interrupts for a specific pin
 */
void hal_ext_int_deshabilitar_int(uint32_t pin) {
    // Find the GPIOTE channel corresponding to the pin
    for (int i = 0; i < GPIOTE_CH_NUM; i++) {
        if (((NRF_GPIOTE->CONFIG[i] & GPIOTE_CONFIG_PSEL_Msk) >> GPIOTE_CONFIG_PSEL_Pos) == pin) {
            // Disable interrupt
            NRF_GPIOTE->INTENCLR = (1 << i);
            // Clear channel configuration
            NRF_GPIOTE->CONFIG[i] = 0;
            break;
        }
    }
}

/**
 * @brief Enable wake-up capability for a pin
 * @see: https://yliu.eng.wayne.edu/teaching/IE5995/Week3_Arduino.pdf
 */
void hal_ext_int_habilitar_despertar(uint32_t pin) {
    // Configure pin to wake up from System OFF mode
    NRF_GPIO->PIN_CNF[pin] = 
        (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos) |
        (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos) |
        (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos);
}

/**
 * @brief Disable wake-up capability for a pin
 * @see: https://yliu.eng.wayne.edu/teaching/IE5995/Week3_Arduino.pdf
 */
void hal_ext_int_deshabilitar_despertar(uint32_t pin) {
    // Disable event detection for wake-up
    NRF_GPIO->PIN_CNF[pin] &= ~GPIO_PIN_CNF_SENSE_Msk;
    NRF_GPIO->PIN_CNF[pin] |= (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
}

/**
 * @brief GPIOTE Interrupt Handler
 */
void GPIOTE_IRQHandler(void) {
    // Determine which pin generated the interrupt
    for (int i = 0; i < GPIOTE_CH_NUM; i++) {
        if (NRF_GPIOTE->EVENTS_IN[i]) {
            NRF_GPIOTE->EVENTS_IN[i] = 0; // Clear event
            
            // Add memory barrier to ensure event is cleared
            volatile uint32_t dummy = NRF_GPIOTE->EVENTS_IN[i];
            (void)dummy;
            
            if (ext_int_callback) {
                // Get pin associated with this GPIOTE channel
                uint32_t pin = (NRF_GPIOTE->CONFIG[i] & GPIOTE_CONFIG_PSEL_Msk) >> GPIOTE_CONFIG_PSEL_Pos;
                ext_int_callback(pin);
            }
        }
    }
} 

/**
 * @brief Get the current state of a gpio pin
 * 
 * @param pin Pin number to check
 * @return uint8_t Current state of the pin
 */
uint8_t hal_ext_int_get_estado_pin(uint32_t pin){
    uint32_t masc = (1UL << pin);
	return (uint8_t)((NRF_GPIO->IN & masc)!=0);
}
