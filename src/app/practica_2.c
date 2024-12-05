/* *****************************************************************************
 * Hardware Project 2024
 * 
 * practica_2.c - LED Blinking Practice Implementation
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
 *   Implementation of Practice 2 module. Contains two versions of LED blinking:
 *   - Version 1: Using busy-wait delay through instruction loop
 *   - Version 2: Using hardware timer for precise timing
 * *****************************************************************************/

#include "drv_tiempo.h"
#include "drv_leds.h"
#include "practica_2.h"

/******************************************************************************
 * Definitions
 *****************************************************************************/
#define RETARDO_MS_BLINK_V2 500

/******************************************************************************
 * LED Blinking Functions
 *****************************************************************************/

/**
 * @brief Basic LED blinking using busy-wait delay
 * 
 * Implements LED blinking using a simple instruction loop for delay.
 * Used in the first session of the practice.
 * 
 * @param id LED identifier to blink
 */
void blink_v1(uint32_t id)
{
	drv_led_encender(id);
	while (1)
	{
		uint32_t volatile tmo;

		tmo = 10000000;
		while (tmo--)
			;
		drv_led_conmutar(id);
	}
}

/**
 * @brief Timer-based LED blinking
 * 
 * Implements LED blinking using hardware timer for precise delays.
 * Used in the second session of the practice.
 * 
 * @param id LED identifier to blink
 */
void blink_v2(uint32_t id)
{

	// NOTE: drv_tiempo_iniciar() already called in main function
	Tiempo_ms_t siguiente_activacion;

	drv_led_encender(id);

	siguiente_activacion = drv_tiempo_actual_ms();

	/* Toggle LEDs. */
	while (1)
	{
		siguiente_activacion += RETARDO_MS_BLINK_V2; // ms
		drv_tiempo_esperar_hasta_ms(siguiente_activacion);
		drv_led_conmutar(id);
	}
}
