/**
 * @file drv_leds.h
 * @ingroup DRV
 * @brief LED Driver Interface
 * @details Interface for the LED driver module. Provides hardware-independent
 *          functions to initialize and control LEDs, including basic operations
 *          like turning on/off and toggling.
 *
 * @defgroup DRV_LEDS LED Driver
 * @ingroup DRV
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

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

/** @} */ // End of DRV_LEDS group

#endif
