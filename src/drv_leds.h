/* *****************************************************************************
 * Hardware Project 2024
 * 
 * drv_leds.h - LED Driver Interface
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
 *   Interface for the LED driver module. Provides hardware-independent
 *   functions to initialize and control LEDs, including basic operations
 *   like turning on/off and toggling.
 * *****************************************************************************/

#ifndef DRV_LEDS
#define DRV_LEDS

#include <stdint.h>

/**
 * @brief Initialize LED driver
 * 
 * Initializes all LEDs, turns them off and returns the number
 * of available LEDs on the platform.
 * 
 * @return uint32_t Number of available LEDs
 */
uint32_t drv_leds_iniciar(void);

/**
 * @brief Turn on an LED
 * 
 * @param id LED identifier (1 to LEDS_NUMBER)
 */
void drv_led_encender(uint32_t id);

/**
 * @brief Turn off an LED
 * 
 * @param id LED identifier (1 to LEDS_NUMBER)
 */
void drv_led_apagar(uint32_t id);

/**
 * @brief Toggle an LED
 * 
 * Switches the LED state from on to off or vice versa
 * 
 * @param id LED identifier (1 to LEDS_NUMBER)
 */
void drv_led_conmutar(uint32_t id);

#if 0
/**
 * Optional high-level functions that might be useful.
 * [NOT IMPLEMENTED]
 */
uint32_t drv_led_estado(uint32_t id);
void drv_leds_encender_todos();
void drv_leds_apagar_todos();
#endif

#endif
