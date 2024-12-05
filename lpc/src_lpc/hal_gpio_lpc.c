/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_gpio_lpc.c - GPIO Hardware Abstraction Layer for LPC2105
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
 *   Implementation of the GPIO Hardware Abstraction Layer for the LPC2105 
 *   microcontroller. This module provides functions to configure and control
 *   GPIO pins, supporting both individual pin and multi-pin operations.
 *   External interrupt handling for buttons is implemented in a separate module.
 * *****************************************************************************/

#include <LPC210x.H>                       /* LPC210x definitions */

#include "hal_gpio.h"

/**
 * @brief Initialize GPIO functionality
 * 
 * Must be called before using any other GPIO functions.
 * Resets all pins as inputs to avoid short circuits.
 */
void hal_gpio_iniciar(void){
  // Reset all pins as inputs (same as reset state)
  IODIR = 0x0; // GPIO Port Direction control register.
				       // Controls the direction of each port pin
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
			IODIR = IODIR & ~masc;
	}
	else if (direccion == HAL_GPIO_PIN_DIR_OUTPUT){
			IODIR = IODIR | masc;
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

	return (IOPIN & masc) >> gpio_inicial;
  	// IOPIN : GPIO Port Pin value register. Contains the state of
	  // the pins configured independently of the direction.
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
	uint32_t temp = IOPIN & ~masc;
	IOPIN = temp | masc_value; 
	// Clears the mask in the IOPIN and changes its bits at once
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
			IODIR = IODIR & ~masc;
	}
	else if (direccion == HAL_GPIO_PIN_DIR_OUTPUT){
			IODIR = IODIR | masc;
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
	return ((IOPIN & masc)!=0);
}


/**
 * @brief Write value to a single GPIO pin
 * 
 * @param gpio GPIO pin to write to
 * @param valor Value to write (0 or 1)
 */
void hal_gpio_escribir(HAL_GPIO_PIN_T gpio, uint32_t valor){
	uint32_t masc = (1UL << gpio);
	
	if ((valor & 0x01) == 0) IOCLR = masc;
	else IOSET = masc;
}
