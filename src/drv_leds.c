/* *****************************************************************************
 * Hardware Project 2024
 * 
 * drv_leds.c - LED Driver Implementation
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
 *   Implementation of the LED driver module. Provides hardware-independent
 *   LED control services. Uses board configuration for:
 *   - LEDS_NUMBER: Number of available LEDs
 *   - LEDS_LIST: Array of LED GPIO pins
 *   - LEDS_ACTIVE_STATE: Active level for LEDs (high/low)
 * *****************************************************************************/

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

//otras???
