/**
 * @file rt_GE.h
 * @ingroup RT_EVENT
 * @brief Real-Time Event Manager
 * @details Provides a publish-subscribe event management system for real-time
 *          applications. Handles event subscription, dispatch, and processing
 *          with support for multiple subscribers per event type.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#ifndef RT_GE
#define RT_GE

#include <stdint.h>
#include "rt_evento_t.h"

/**
 * @brief Initialize the event manager
 * @param monitor Monitor configuration
 */
void rt_GE_iniciar(uint32_t monitor);

/**
 * @brief Event dispatcher main loop
 */
void rt_GE_lanzador(void);

/**
 * @brief Subscribe a callback to an event
 * @param evento Event type to subscribe to
 * @param f_callback Callback function to be called when event occurs
 */
void svc_GE_suscribir(EVENTO_T evento, void (*f_callback)(uint32_t, uint32_t));

/**
 * @brief Unsubscribe a callback from an event
 * @param evento Event type to unsubscribe from
 * @param f_callback Callback function to unsubscribe
 */
void svc_GE_cancelar(EVENTO_T evento, void (*f_callback)(uint32_t, uint32_t));

#endif // RT_GE
