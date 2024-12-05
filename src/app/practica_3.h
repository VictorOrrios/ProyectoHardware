/* *****************************************************************************
 * Hardware Project 2024
 * 
 * practica_3.h - Event-Based LED Control Interface
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
 *   Interface for Practice 3 module. Provides functions to demonstrate LED
 *   control using event-based programming and power management.
 * *****************************************************************************/

#ifndef PRACTICA_3
#define PRACTICA_3

#include <stdint.h>

/**
 * @brief Timer interrupt-based LED blinking
 * 
 * Implements LED blinking using timer interrupts and power management.
 * 
 * @param id LED identifier to blink
 */
void blink_v3(uint32_t id);

/**
 * @brief Event queue-based LED blinking
 * 
 * Implements LED blinking using the runtime system with event queue
 * management and scheduler for event processing.
 * 
 * @param id LED identifier to blink
 */
void blink_v4(uint32_t id);

#endif // PRACTICA_3
