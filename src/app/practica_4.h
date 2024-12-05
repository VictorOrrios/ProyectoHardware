/* *****************************************************************************
 * Hardware Project 2024
 * 
 * practica_4.h - Advanced LED Control Interface
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
 *   Interface for Practice 4 module. Provides functions for advanced LED
 *   control patterns including limited blink sequences and interactive
 *   LED patterns using buttons.
 * *****************************************************************************/

#ifndef PRACTICA_4
#define PRACTICA_4

#include <stdint.h>

/**
 * @brief Enhanced version of blink_v3 with limited blinks
 * 
 * Blinks LED for a fixed number of times then enters deep sleep mode.
 * 
 * @param id LED identifier to blink
 */
void blink_v3_bis(uint32_t id);

/**
 * @brief Interactive LED pattern game
 * 
 * Implements an interactive LED pattern where LEDs are controlled
 * based on button presses and timing.
 * 
 * @param retardoSpawnInicial Initial spawn delay in milliseconds
 * @param num_leds Number of LEDs available
 * @param num_botones Number of buttons available
 */
void bit_counterstrike_launcher(uint32_t retardoSpawnInicial, uint32_t num_leds, uint32_t num_botones);

#endif // PRACTICA_4
