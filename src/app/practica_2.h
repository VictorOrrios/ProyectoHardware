/**
 * @file practica_2.h
 * @ingroup APP
 * @brief LED Blinking Practice Interface
 * @details Interface for Practice 2 module. Provides functions to demonstrate LED
 *          blinking using different timing methods:
 *          - Busy-wait delay implementation
 *          - Timer-based precise timing
 *          - Power-efficient operation
 *
 * @defgroup APP_BLINK LED Blinking Practice
 * @ingroup APP
 * @details This module demonstrates basic LED control techniques:
 *          - Basic busy-wait timing control
 *          - Hardware timer utilization
 *          - Power consumption considerations
 *          - Interrupt handling basics
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

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

/** @} */ // End of DRV_BUTTONS group

#endif // PRACTICA_2
