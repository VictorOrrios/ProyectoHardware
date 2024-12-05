/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_gpio_nrf.h - GPIO Hardware Abstraction Layer Header for nRF52840
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
 *   Header file for the GPIO Hardware Abstraction Layer for nRF52840.
 *   Defines the interface for GPIO pin configuration and control.
 * *****************************************************************************/

#ifndef HAL_GPIO
#define HAL_GPIO

#include <stdint.h>

/**
 * @brief GPIO pin direction enumeration
 */
enum {
    HAL_GPIO_PIN_DIR_INPUT = 0,
    HAL_GPIO_PIN_DIR_OUTPUT = 1,
} typedef hal_gpio_pin_dir_t;

/**
 * @brief GPIO pin type definition
 */
typedef uint32_t HAL_GPIO_PIN_T;

/**
 * @brief Initialize GPIO functionality
 * 
 * Must be called before using any other GPIO functions.
 * Resets all pins as inputs to avoid short circuits.
 */
void hal_gpio_iniciar(void);

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
            uint8_t num_bits, hal_gpio_pin_dir_t direccion);

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
uint32_t hal_gpio_leer_n(HAL_GPIO_PIN_T gpio_inicial, uint8_t num_bits);

/**
 * @brief Write value to multiple consecutive GPIO pins
 * 
 * @param bit_inicial First GPIO pin to write to
 * @param num_bits Number of consecutive pins to write
 * @param valor Value to write (if value is larger than num_bits, only the least significant bits are used)
 */
void hal_gpio_escribir_n(HAL_GPIO_PIN_T bit_inicial, 
            uint8_t num_bits, uint32_t valor);

/* *****************************************************************************
 * Single GPIO pin access functions (optimized)
 */

/**
 * @brief Configure direction for a single GPIO pin
 * 
 * @param gpio GPIO pin to configure
 * @param direccion Direction to set (input or output)
 */
void hal_gpio_sentido(HAL_GPIO_PIN_T gpio, hal_gpio_pin_dir_t direccion);

/**
 * @brief Read value from a single GPIO pin
 * 
 * @param gpio GPIO pin to read
 * @return uint32_t Boolean value (0 or non-zero)
 */
uint32_t hal_gpio_leer(HAL_GPIO_PIN_T gpio);

/**
 * @brief Write value to a single GPIO pin
 * 
 * @param gpio GPIO pin to write to
 * @param valor Value to write (0 or 1)
 */
void hal_gpio_escribir(HAL_GPIO_PIN_T gpio, uint32_t valor);

#endif
