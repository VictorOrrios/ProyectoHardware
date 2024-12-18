/**
 * @file rt_fifo.h
 * @ingroup RT_FIFO
 * @brief Real-Time FIFO Queue Manager
 * @details Implements a real-time FIFO queue for event management with
 *          thread-safe operations, overflow protection and statistical tracking.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#ifndef RT_FIFO
#define RT_FIFO

#include "hal_gpio.h"
#include "rt_evento_t.h"
#include "drv_tiempo.h"

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
