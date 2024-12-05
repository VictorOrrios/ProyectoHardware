/* *****************************************************************************
 * Hardware Project 2024
 * 
 * rt_GE.c - Real-Time Event Manager Implementation
 * 
 * Description:
 *   Implements the event management system defined in rt_GE.h. Provides
 *   a central event dispatch mechanism with support for multiple subscribers
 *   per event type. Handles power management through inactivity detection
 *   and includes protection against subscription overflow.
 * 
 * Implementation Notes:
 *   - Uses array-based storage for subscribers
 *   - Implements power management through idle/sleep modes
 *   - Provides automatic inactivity detection
 *   - Includes overflow protection for subscriber lists
 * *****************************************************************************/

#include "rt_GE.h"
#include "rt_fifo.h"
#include "drv_consumo.h"
#include "board.h"
#include "drv_monitor.h"
#include "svc_alarma.h"
#include "svc_log.h"
#include <string.h>

typedef struct
{
    void (*callback)();
    EVENTO_T evento;
} Suscriptor;

static Suscriptor suscriptores[EVENT_TYPES][rt_GE_MAX_SUSCRITOS];
static uint32_t num_suscriptores[EVENT_TYPES];
static uint32_t monitor_overflow_global;

// Lista de eventos de usuario definida en rt_evento_t.h
static const EVENTO_T eventos_usuario[] = ev_USUARIO;

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
            // rt_GE_tratar(evento, auxData);
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
						// LOG_DEBUG("Lanzador entrando en modo de espera...");
            drv_consumo_esperar();
            // exiting idle mode
        }
				// svc_log_procesar();
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
