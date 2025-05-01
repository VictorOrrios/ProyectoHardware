/**
 * @file rt_GE.c
 * @ingroup RT_EVENT
 * @brief Real-Time Event Manager Implementation
 * @details Implements the event management system defined in rt_GE.h. Provides
 *          a central event dispatch mechanism with support for multiple subscribers
 *          per event type.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "rt_GE.h"
#include "rt_fifo.h"
#include "drv_consumo.h"
#include "rt_evento_t.h"
#include "drv_monitor.h"
#include "svc_alarma.h"
#include "svc_log.h"
#include <string.h>

/**
 * @brief Maximum number of subscribers per event
 */
#define rt_GE_MAX_SUSCRITOS 4

/**
 * @brief Inactivity timeout in milliseconds
 */
#define INACTIVITY_TIMEOUT_MS 20000

/** 
 * @brief Subscriber information structure
 */
typedef struct {
    void (*callback)();     ///< Callback function to be called when event occurs
    EVENTO_T evento;        ///< Event type this subscriber is interested in
} Suscriptor;

/** @brief Array of subscribers for each event type */
static Suscriptor suscriptores[EVENT_TYPES][rt_GE_MAX_SUSCRITOS];

/** @brief Number of subscribers for each event type */
static uint32_t num_suscriptores[EVENT_TYPES];
static uint32_t monitor_overflow_global;

// Lista de eventos de usuario definida en rt_evento_t.h
static const EVENTO_T eventos_usuario[] = ev_USUARIO;

void rt_GE_tratar(uint32_t evento, uint32_t auxiliar); // Early definition, gets implemented later

/**
 * @brief Initialize the event manager
 * @param monitor Monitor configuration
 */
void rt_GE_iniciar(uint32_t monitor)
{
    monitor_overflow_global = monitor;

    memset(suscriptores, 0, sizeof(suscriptores));
    memset(num_suscriptores, 0, sizeof(num_suscriptores));

    for (uint32_t i = 0; i < ev_NUM_EV_USUARIO; i++)
    {
        svc_GE_suscribir(eventos_usuario[i], rt_GE_tratar);
    }

    svc_GE_suscribir(ev_INACTIVIDAD, rt_GE_tratar);
}

/**
 * @brief Event dispatcher main loop
 */
void rt_GE_lanzador(void)
{
    EVENTO_T evento;
    uint32_t auxData;
    Tiempo_us_t timestamp;

    // Activar alarma de inactividad (20 segundos)
    svc_alarma_activar(svc_alarma_codificar(0, INACTIVITY_TIMEOUT_MS), ev_INACTIVIDAD, 0);

    while (1)
    {
        if (rt_FIFO_extraer(&evento, &auxData, &timestamp))
        {
            // Process event
            for (uint32_t i = 0; i < num_suscriptores[evento]; i++)
            {
                if (suscriptores[evento][i].callback != NULL)
                {
                    suscriptores[evento][i].callback(evento, auxData);
                }
            }
        }
        else
        {
            // No events, enter idle mode
            drv_consumo_esperar();
            // exiting idle mode
        }
    }
}

/**
 * @brief Subscribe a callback to an event
 * @param evento Event type to subscribe to
 * @param f_callback Callback function to be called when event occurs
 */
void svc_GE_suscribir(EVENTO_T evento, void (*f_callback)())
{
    if (num_suscriptores[evento] < rt_GE_MAX_SUSCRITOS)
    {
        suscriptores[evento][num_suscriptores[evento]].callback = f_callback;
        suscriptores[evento][num_suscriptores[evento]].evento = evento;
        num_suscriptores[evento]++; // NOTE: possible race conditions?
    }
    else
    {
        drv_monitor_marcar(monitor_overflow_global);
				LOG_ERROR_F("Max number of suscribers in event %d reached",evento);
				svc_log_procesar();
        while (1)
            ;
    }
}

/**
 * @brief Unsubscribe a callback from an event
 * @param evento Event type to unsubscribe from
 * @param f_callback Callback function to unsubscribe
 */
void svc_GE_cancelar(EVENTO_T evento, void (*f_callback)())
{
    for (uint32_t i = 0; i < num_suscriptores[evento]; i++)
    {
        if (suscriptores[evento][i].callback == f_callback)
        {
            // Shift remaining subscribers
            for (uint32_t j = i; j < num_suscriptores[evento] - 1; j++)
            {
                suscriptores[evento][j] = suscriptores[evento][j + 1];
            }
            num_suscriptores[evento]--;
            break;
        }
    }
}

/**
 * @brief Process an event
 * @param evento Event type to process
 * @param auxiliar Additional event data
 */
void rt_GE_tratar(uint32_t evento, uint32_t auxiliar)
{
    // Handle inactivity event
    if (evento == ev_INACTIVIDAD)
    {
        drv_consumo_dormir();
        return;
    }

    // For user events, reset inactivity timer
    // Check if it's a user event by comparing against the user events array
    for (uint32_t i = 0; i < ev_NUM_EV_USUARIO; i++)
    {
        if (evento == eventos_usuario[i])
        {
            // Reprogram inactivity timer as one-shot alarm (MSB = 0) (svc_alarma_activar
            //  is not a subscribed function)
            svc_alarma_activar(svc_alarma_codificar(0, INACTIVITY_TIMEOUT_MS), ev_INACTIVIDAD, 0);
            break;
        }
    }
}
