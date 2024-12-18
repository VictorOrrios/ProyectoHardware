/**
 * @file practica_3.h
 * @ingroup APP
 * @brief Event-Based LED Control Interface
 * @details Interface for Practice 3 module. Provides functions to demonstrate LED
 *          control using event-based programming and power management.
 *
 * @defgroup APP_EVENT Event-Based LED Control
 * @ingroup APP
 * @details This module implements event-driven LED control:
 *          - Timer interrupt handling
 *          - Event queue management
 *          - Power-efficient scheduling
 *          - Runtime system integration
 * @{
 */

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

/** @} */ // End of DRV_BUTTONS group

#endif // PRACTICA_3
