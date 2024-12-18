/**
 * @file hal_gpio.h
 * @ingroup HAL
 * @brief GPIO HAL Interface
 * @details Interface for the GPIO Hardware Abstraction Layer.
 *          Provides functions to configure and access GPIO pins
 *          independently of the specific hardware.
 *
 * @defgroup HAL_GPIO GPIO HAL
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
 * @brief Initialize GPIO system
 * 
 * Must be called before using any other GPIO functions.
 * Configures all pins as inputs by default to prevent short circuits.
 */
void hal_gpio_iniciar(void);

/**************************************************/
/* Multi-bit GPIO access functions                 */
/**************************************************/

/**
 * @brief Configure direction for multiple GPIO pins
 * 
 * @param gpio_inicial First GPIO pin to configure
 * @param num_bits Number of consecutive pins to configure
 * @param direccion Direction (input/output) to set
 */
void hal_gpio_sentido_n(HAL_GPIO_PIN_T gpio_inicial, 
                       uint8_t num_bits, hal_gpio_pin_dir_t direccion);

/**
 * @brief Read value from multiple GPIO pins
 * 
 * Returns an integer with the value of the specified bits.
 * Example:
 * - Pin values: 0x0F0FAFF0
 * - Initial bit: 12, num_bits: 4
 * - Return value: 10 (reads bits 12-15)
 * 
 * @param gpio_inicial First GPIO pin to read
 * @param num_bits Number of consecutive pins to read
 * @return uint32_t Value read from pins
 */
uint32_t hal_gpio_leer_n(HAL_GPIO_PIN_T gpio_inicial, uint8_t num_bits);

/**
 * @brief Write value to multiple GPIO pins
 * 
 * If value cannot be represented in the specified number of bits,
 * only the least significant bits will be written starting from gpio_inicial.
 * 
 * @param bit_inicial First GPIO pin to write
 * @param num_bits Number of consecutive pins to write
 * @param valor Value to write to pins
 */
void hal_gpio_escribir_n(HAL_GPIO_PIN_T bit_inicial, 
                        uint8_t num_bits, uint32_t valor);

/**************************************************/
/* Single GPIO access functions                    */
/**************************************************/

/**
 * @brief Configure direction for a single GPIO pin
 * 
 * @param gpio GPIO pin to configure
 * @param direccion Direction (input/output) to set
 */
void hal_gpio_sentido(HAL_GPIO_PIN_T gpio, hal_gpio_pin_dir_t direccion);

/**
 * @brief Read value from a single GPIO pin
 * 
 * @param gpio GPIO pin to read
 * @return uint32_t 0 if pin is low, non-zero if pin is high
 */
uint32_t hal_gpio_leer(HAL_GPIO_PIN_T gpio);

/**
 * @brief Write value to a single GPIO pin
 * 
 * @param gpio GPIO pin to write
 * @param valor Value to write (0 for low, non-zero for high)
 */
void hal_gpio_escribir(HAL_GPIO_PIN_T gpio, uint32_t valor);

#endif
