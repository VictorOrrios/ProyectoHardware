/* *****************************************************************************
 * Hardware Project 2024
 * 
 * practica_2.h - LED Blinking Practice Interface
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
 *   Interface for Practice 2 module. Provides functions to demonstrate LED
 *   blinking using different timing methods: busy-wait and timer-based.
 * *****************************************************************************/

#ifndef PRACTICA_2
#define PRACTICA_2

#include <stdint.h>

/**
 * @brief Basic LED blinking using busy-wait delay
 * 
 * Implements LED blinking using a simple instruction loop for delay.
 * Used in the first session of the practice.
 * 
 * @param id LED identifier to blink
 */
void blink_v1(uint32_t id);

/**
 * @brief Timer-based LED blinking
 * 
 * Implements LED blinking using hardware timer for precise delays.
 * Used in the second session of the practice.
 * 
 * @param id LED identifier to blink
 */
void blink_v2(uint32_t id);

#endif // PRACTICA_2
