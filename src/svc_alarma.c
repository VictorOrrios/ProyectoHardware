/* *****************************************************************************
 * Hardware Project 2024
 *
 * svc_alarma.c - Alarm Service Implementation
 *
 * Description:
 *   Implements the alarm service defined in svc_alarma.h. Manages multiple
 *   concurrent alarms using an array-based storage system with support for
 *   both periodic and one-shot alarms.
 *
 * Implementation Notes:
 *   - Uses thread-mode-only execution (no critical sections needed)
 *   - Implements efficient next-trigger calculation
 *   - Supports alarm reprogramming and cancellation
 *   - Includes overflow protection mechanisms
 * *****************************************************************************/

#include "svc_alarma.h"
#include "rt_GE.h"
#include "drv_tiempo.h"
#include "drv_monitor.h"
#include "rt_FIFO.h"
#include "rt_evento_t.h"
#include "svc_log.h"
#include <string.h>

typedef struct
{
  uint32_t retardo_ms;
  uint32_t ID_evento;
  uint32_t auxData;
  uint32_t active;
  uint32_t periodic;
  uint32_t next_trigger;
} Alarma;

static Alarma alarmas[svc_ALARMAS_MAX];
static void (*callback_encolar_global)();
static uint32_t monitor_overflow;

/**
 * @brief Initialize the alarm service
 * @param overflow Monitor ID for overflow detection
 * @param f_callback Function to call when alarm triggers
 * @param ID_evento Event type to generate for alarm notifications
 * @note ID_evento SHOULD BE ev_T_PERIODICO, and f_callback SHOULD
 *       BE rt_FIFO_encolar (read the script -> practica4)
 */
void svc_alarma_iniciar(uint32_t overflow, void (*f_callback)(), uint32_t ID_evento)
{
  memset(alarmas, 0, sizeof(alarmas));
  callback_encolar_global = f_callback;
  monitor_overflow = overflow;
  // ID_evento_global = ID_evento;
  // num_alarmas_activas = 0;
  // programacion_periodica_ideal = 0;

  // Subscribe to periodic timer events
  svc_GE_suscribir((EVENTO_T)ID_evento, svc_alarma_tratar);
	
	// TMP
	drv_tiempo_periodico_ms(1, f_callback, ID_evento);
}

/**
 * @brief Activate or reprogram an alarm
 * @param retardo_ms Delay in milliseconds (MSB indicates periodic/one-shot)
 * @param ID_evento Event ID for this alarm
 * @param auxData Additional data to pass with the event
 */
void svc_alarma_activar(uint32_t retardo_ms, uint32_t ID_evento, uint32_t auxData)
{
    uint32_t idx;
    uint32_t current_time = drv_tiempo_actual_ms();
    uint32_t is_periodic = retardo_ms & 0x80000000;
    retardo_ms &= 0x7FFFFFFF; // Clear periodic bit

    // Find free slot or existing alarm with same ID
    for (idx = 0; idx < svc_ALARMAS_MAX; idx++)
    {
        if (!alarmas[idx].active || alarmas[idx].ID_evento == ID_evento)
        {
            break;
        }
    }

    // Check if we found a slot, if not signal overflow and halt
    if (idx >= svc_ALARMAS_MAX)
    {
        drv_monitor_marcar(monitor_overflow);
				LOG_ERROR_F("Max number alarms reached while trying to program one for event %d",ID_evento);
        LOG_ERROR("Currently active alarms:");
        for (uint32_t i = 0; i < svc_ALARMAS_MAX; i++)
        {
          if (alarmas[i].active)
          {
            LOG_ERROR_F("Alarm %d: event %d", i, alarmas[i].ID_evento);
          }
        }
				svc_log_procesar();
        while (1)
            ; // Infinite loop
    }

    Alarma newAlarma;
    newAlarma.retardo_ms = retardo_ms;
    newAlarma.ID_evento = ID_evento;
    newAlarma.auxData = auxData;
    newAlarma.active = (retardo_ms > 0);
    newAlarma.periodic = is_periodic;
    newAlarma.next_trigger = current_time + retardo_ms;

    alarmas[idx] = newAlarma;
}

/**
 * @brief Process alarm events
 * @param evento Event type
 * @param aux Additional event data
 */
void svc_alarma_tratar(uint32_t evento, uint32_t aux)
{
    uint32_t current_time = drv_tiempo_actual_ms();

    for (uint32_t i = 0; i < svc_ALARMAS_MAX; i++)
    {
        Alarma readAlarma = alarmas[i];
        if (readAlarma.active && current_time >= readAlarma.next_trigger)
        {
            // Encolar el evento directamente en la FIFO

            if (readAlarma.periodic)
            {
                // Reprogramar alarma periódica
                alarmas[i].next_trigger = current_time + readAlarma.retardo_ms;
            }
            else
            {
                // Desactivar alarma de un solo disparo
                alarmas[i].active = 0;
            }

            callback_encolar_global(readAlarma.ID_evento, readAlarma.auxData);
        }
    }
}

/**
 * @brief Encode a delay and periodic flag into a single 32-bit value
 *
 * @param periodico true if alarm is periodic, false if one-shot
 * @param retardo_ms delay time in milliseconds (must be < 0x80000000)
 * @return uint32_t encoded value (MSB = periodic flag, rest = delay)
 */
uint32_t svc_alarma_codificar(uint8_t periodico, uint32_t retardo_ms)
{
  // Verify retardo_ms doesn't use the MSB
  if (retardo_ms & 0x80000000)
  {
    drv_monitor_marcar(monitor_overflow);
		LOG_ERROR_F("Delay to big to process %d",retardo_ms);
		svc_log_procesar();
    while (1)
      ; // Halt system
  }

  // Encode: MSB = periodic flag, rest = delay
  return (periodico ? 0x80000000 : 0) | retardo_ms;
}
