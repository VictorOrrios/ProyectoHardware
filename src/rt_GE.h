/* *****************************************************************************
 * Hardware Project 2024
 * 
 * rt_GE.h - Real-Time Event Manager
 * 
 * Description:
 *   Provides a publish-subscribe event management system for real-time
 *   applications. Handles event subscription, dispatch, and processing
 *   with support for multiple subscribers per event type. Includes
 *   power management through inactivity detection.
 * 
 * Key Features:
 *   - Multiple subscriber support per event
 *   - Automatic inactivity detection and power management
 *   - Dynamic subscription/unsubscription
 *   - Event dispatch with auxiliary data
 * *****************************************************************************/

#ifndef RT_GE
#define RT_GE

#include <stdint.h>
#include "rt_evento_t.h"

/**
 * @brief Maximum number of subscribers per event
 */
#define rt_GE_MAX_SUSCRITOS 4

/**
 * @brief Inactivity timeout in milliseconds
 */
#define INACTIVITY_TIMEOUT_MS 20000

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

/**
 * @brief Process an event
 * @param evento Event type to process
 * @param auxiliar Additional event data
 */
void rt_GE_tratar(uint32_t evento, uint32_t auxiliar);

#endif // RT_GE
