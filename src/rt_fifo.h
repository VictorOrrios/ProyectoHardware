/* *****************************************************************************
 * Hardware Project 2024
 * 
 * rt_fifo.h - Real-Time FIFO Queue Manager
 * 
 * Description:
 *   Implements a real-time FIFO (First In First Out) queue for event management.
 *   This module provides thread-safe operations for enqueueing and dequeuing
 *   events, with overflow protection and statistical tracking. The queue is
 *   designed for real-time event processing with timestamp tracking.
 * 
 * Key Features:
 *   - Fixed-size circular buffer implementation
 *   - Overflow detection and handling
 *   - Event timestamping
 *   - Statistical tracking of event counts
 *   - Thread-safe operations through critical sections
 * *****************************************************************************/

#ifndef RT_FIFO
#define RT_FIFO

#include "hal_gpio.h"
#include "rt_evento_t.h"
#include "drv_tiempo.h"
#include <stdint.h>
#include "drv_sc.h"

/**
 * @brief Structure that defines an event in the system
 * 
 * Each event contains an identifier, auxiliary data, and a timestamp
 * indicating when the event was queued.
 */
typedef struct{
    EVENTO_T ID_EVENTO; ///< Event type identifier
    uint32_t auxData;   ///< Event-specific auxiliary data
    Tiempo_us_t TS;     ///< Timestamp in microseconds
} EVENTO;

/**
 * @brief Maximum size of the event queue
 * 
 * Defines how many events can be pending simultaneously.
 * @warning Must be a power of 2 to optimize modulo operations
 */
#define EVENT_QUEUE_SIZE 64

/**
 * @brief Initialize the event queue
 * 
 * @param pin_monitor_overflow GPIO pin that will be activated in case of overflow
 * @note The pin must be previously configured as output
 */
void rt_FIFO_inicializar(HAL_GPIO_PIN_T pin_monitor_overflow);

/**
 * @brief Queue a new event in the FIFO queue
 * 
 * @param ID_evento Event identifier to queue
 * @param auxData Auxiliary data associated with the event
 * 
 * @warning If the queue is full, the overflow pin is marked and the system
 * enters an infinite loop as a safety measure
 */
void rt_FIFO_encolar(uint32_t ID_evento, uint32_t auxData);

/**
 * @brief Extract the next event from the FIFO queue
 * 
 * @param[out] ID_evento Pointer where the extracted event ID will be stored
 * @param[out] auxData Pointer where the auxiliary data will be stored
 * @param[out] TS Pointer where the timestamp will be stored
 * 
 * @return Number of pending events in the queue after extraction
 *         0 if the queue is empty
 */
uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t *auxData, Tiempo_us_t *TS);

/**
 * @brief Get statistics about processed events
 * 
 * @param ID_evento Event type to get statistics for
 * 
 * @return For ev_VOID: total sum of all queued events
 *         For other types: total number of times that type has been queued
 */
uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento);

#endif
