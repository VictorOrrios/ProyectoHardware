/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_gpio_nrf.c - GPIO Hardware Abstraction Layer for nRF52840
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
 *   Implementation of the GPIO Hardware Abstraction Layer for the nRF52840
 *   microcontroller. This module provides functions to configure and control
 *   GPIO pins, supporting both individual pin and multi-pin operations.
 * *****************************************************************************/
   
#include "nrf.h"
#include "board.h"
#include "hal_gpio.h"

/**
 * @brief Initialize GPIO functionality
 * 
 * Must be called before using any other GPIO functions.
 * Resets all pins as inputs to avoid short circuits.
 */
void hal_gpio_iniciar(void){
  	// Reinitialize all pins as input (same as reset)
	NRF_GPIO->DIR=0x0;
}

/* *****************************************************************************
 * Multi-pin GPIO access functions
 */

/**
 * @brief Configure direction for multiple consecutive GPIO pins
 * 
 * @param gpio_inicial First GPIO pin to configure
 * @param num_bits Number of consecutive pins to configure
 * @param direccion Direction to set (input or output)
 */
void hal_gpio_sentido_n(HAL_GPIO_PIN_T gpio_inicial, 
			uint8_t num_bits, hal_gpio_pin_dir_t direccion){
				
	uint32_t masc = ((1 << num_bits) - 1) << gpio_inicial;
	if (direccion == HAL_GPIO_PIN_DIR_INPUT){
			NRF_GPIO->DIR &= ~masc;
	}
	else if (direccion == HAL_GPIO_PIN_DIR_OUTPUT){
			NRF_GPIO->DIR |= masc;
	}									
}												

/**
 * @brief Read value from multiple consecutive GPIO pins
 * 
 * @param gpio_inicial First GPIO pin to read
 * @param num_bits Number of consecutive pins to read
 * @return uint32_t Value read from the pins
 * 
 * Example:
 *   - Pin values: 0x0F0FAFF0
 *   - gpio_inicial: 12, num_bits: 4
 *   - Return value: 10 (reads bits 12-15)
 */
uint32_t hal_gpio_leer_n(HAL_GPIO_PIN_T gpio_inicial, uint8_t num_bits){
	
	uint32_t masc = ((1 << num_bits) - 1) << gpio_inicial;

	return (NRF_GPIO->IN & masc) >> gpio_inicial;
}

/**
 * @brief Write value to multiple consecutive GPIO pins
 * 
 * @param bit_inicial First GPIO pin to write to
 * @param num_bits Number of consecutive pins to write
 * @param valor Value to write (if value is larger than num_bits, only the least significant bits are used)
 */
void hal_gpio_escribir_n(HAL_GPIO_PIN_T bit_inicial, 
			uint8_t num_bits, uint32_t valor){
	uint32_t masc_value = (valor & ((1 << num_bits) - 1)) << bit_inicial;
	uint32_t masc = ((1 << num_bits) - 1) << bit_inicial;
	uint32_t temp = NRF_GPIO->IN & ~masc;
	NRF_GPIO->OUT = temp | masc_value;
}

/* *****************************************************************************
 * Single GPIO pin access functions (optimized)
 */

/**
 * @brief Configure direction for a single GPIO pin
 * 
 * @param gpio GPIO pin to configure
 * @param direccion Direction to set (input or output)
 */
void hal_gpio_sentido(HAL_GPIO_PIN_T gpio, hal_gpio_pin_dir_t direccion){
	uint32_t masc = (1UL << gpio);
	if (direccion == HAL_GPIO_PIN_DIR_INPUT){
			NRF_GPIO->DIRCLR = masc;
			NRF_GPIO->PIN_CNF[gpio] |= 0b10;
	}
	else if (direccion == HAL_GPIO_PIN_DIR_OUTPUT){
		NRF_GPIO->DIRSET = masc;
		NRF_GPIO->PIN_CNF[gpio] &= ~0b10;
	}
}

/**
 * @brief Read value from a single GPIO pin
 * 
 * @param gpio GPIO pin to read
 * @return uint32_t Boolean value read from the pin (0 or 1)
 */
uint32_t hal_gpio_leer(HAL_GPIO_PIN_T gpio){
	uint32_t masc = (1UL << gpio);
	return ((NRF_GPIO->IN & masc)!=0);
}

/**
 * @brief Write value to a single GPIO pin
 * 
 * @param gpio GPIO pin to write to
 * @param valor Value to write (0 or 1)
 */
void hal_gpio_escribir(HAL_GPIO_PIN_T gpio, uint32_t valor){
	uint32_t masc = (1UL << gpio);
	
	if ((valor & 0x01) != 0) NRF_GPIO->OUTSET = masc;
	else NRF_GPIO->OUTCLR = masc;
}
