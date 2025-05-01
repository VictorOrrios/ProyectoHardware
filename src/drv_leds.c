/**
 * @file drv_leds.c
 * @ingroup DRV_LEDS
 * @brief LED Driver Implementation
 * @details Implementation of LED control operations using board-specific configurations.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "hal_gpio.h"
#include "drv_leds.h"
#include "board.h"

#if LEDS_NUMBER > 0
	static const uint8_t led_list[LEDS_NUMBER] = LEDS_LIST;
#endif

/**
 * @brief Initialize LED driver
 * 
 * Initializes all LEDs, turns them off and returns the number
 * of available LEDs on the platform.
 * 
 * @return uint32_t Number of available LEDs
 */
uint32_t drv_leds_iniciar(){
	#if LEDS_NUMBER > 0
		for (uint32_t i = 0; i < LEDS_NUMBER; ++i) 			{
			hal_gpio_sentido(led_list[i], HAL_GPIO_PIN_DIR_OUTPUT);
			drv_led_apagar(i+1);
		}
  #endif //LEDS_NUMBER > 0	
	
	return LEDS_NUMBER;  //definido en board_xxx.h en cada placa... 
}

/**
 * @brief Turn on an LED
 * 
 * @param id LED identifier (1 to LEDS_NUMBER)
 */
void drv_led_encender(uint32_t id){
	#if LEDS_NUMBER > 0
		if ((id <= LEDS_NUMBER) && (id >0)) hal_gpio_escribir(led_list[id-1], LEDS_ACTIVE_STATE);
  #endif //LEDS_NUMBER > 0		
}

/**
 * @brief Turn off an LED
 * 
 * @param id LED identifier (1 to LEDS_NUMBER)
 */
void drv_led_apagar(uint32_t id){
	#if LEDS_NUMBER > 0
		if ((id <= LEDS_NUMBER) && (id >0)) hal_gpio_escribir(led_list[id-1], ~LEDS_ACTIVE_STATE);
  #endif //LEDS_NUMBER > 0	
}

/**
 * @brief Toggle an LED
 * 
 * Switches the LED state from on to off or vice versa
 * 
 * @param id LED identifier (1 to LEDS_NUMBER)
 */
void drv_led_conmutar(uint32_t id){
	#if LEDS_NUMBER > 0
		if ((id <= LEDS_NUMBER) && (id >0)){
			unsigned int foo = hal_gpio_leer(led_list[id-1]);
			hal_gpio_escribir(led_list[id-1], ~hal_gpio_leer(led_list[id-1]));
		}
  #endif //LEDS_NUMBER > 0		
}
