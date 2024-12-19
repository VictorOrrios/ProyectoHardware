/**
 * @file svc_alarma.c
 * @ingroup SVC_ALARM
 * @brief Alarm Service Implementation
 * @details Implements the alarm service defined in svc_alarma.h. Manages multiple
 *          concurrent alarms using an array-based storage system with support for
 *          both periodic and one-shot alarms.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "svc_alarma.h"
#include "rt_GE.h"
#include "drv_tiempo.h"
#include "drv_monitor.h"
#include "rt_FIFO.h"
#include "rt_evento_t.h"
#include "svc_log.h"
#include <string.h>

#define PERIODIC_MASK 0x80000000

/**
 * @brief Maximum number of concurrent alarms
 */
#define svc_ALARMAS_MAX 4

/**
 * @brief Alarm structure representing a single alarm instance
 */
typedef struct
{
    uint32_t retardo_ms;    ///< Delay in milliseconds
    uint32_t ID_evento;     ///< Event ID to trigger when alarm expires
    uint32_t auxData;       ///< Additional data to pass with event
    uint32_t active;        ///< Flag indicating if alarm is active
    uint32_t periodic;      ///< Flag indicating if alarm is periodic
    uint32_t next_trigger;  ///< Timestamp of next trigger in milliseconds
} Alarma;

/** @brief Array of alarm slots */
static Alarma alarmas[svc_ALARMAS_MAX];

/** @brief Callback function for alarm events */
static void (*callback_encolar_global)();

/** @brief Monitor ID for overflow detection */
static uint32_t monitor_overflow;

/** @brief Event type for alarm notifications */
static uint32_t ID_evento_global;

/** @brief Count of currently active alarms */
static uint8_t num_alarmas_activas;

/** @brief Flag indicating if periodic scheduling is optimized */
static uint8_t programacion_periodica_ideal;

/** @brief Current timeout value for next trigger */
static int32_t next_trigger_timeout;

/**
 * @brief Process alarm events and update alarm states
 * @param evento Event type (should be ev_T_PERIODICO)
 * @param aux Auxiliary data (unused)
 */
void svc_alarma_tratar(uint32_t evento, uint32_t aux);

/**
 * @brief Calculate time until next alarm trigger
 * @param timestamp Current system time in milliseconds
 * @return Time until next trigger, or -1 if no active alarms
 */
int32_t get_time_to_min_next_trigger(uint32_t timestamp)
{
  // Set initial value to -1, since some alarmas[i].next_trigger
  // might be 0 (alarm set just when starting the system)
  int32_t min = -1;
  for (uint8_t i = 0; i < svc_ALARMAS_MAX; i++)
  {
    if (alarmas[i].active && (min == -1 || alarmas[i].next_trigger < min))
    {
      min = alarmas[i].next_trigger;
    }
  }
  if (min != -1)
  {
    if (min <= timestamp)
    {
      min = -2;
    }
    else
    {
      // Next trigger is at least 1ms away (looking ahead in time) from current moment
      min -= timestamp;
    }
  }
  return min;
}

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

  ID_evento_global = ID_evento;
  num_alarmas_activas = 0;
  programacion_periodica_ideal = 0;
  next_trigger_timeout = -1;
	
	// To make sure very alarm is set to inactive
	for(uint8_t i = 0; i<svc_ALARMAS_MAX; i++){
		alarmas[i].active = 0;
	}

  // Subscribe to periodic timer events
  svc_GE_suscribir((EVENTO_T)ID_evento, svc_alarma_tratar);
	
}

void svc_alarma_activar(uint32_t retardo_ms, uint32_t ID_evento, uint32_t auxData)
{
  uint32_t idx;
  uint32_t current_time = drv_tiempo_actual_ms();
  uint32_t is_periodic = retardo_ms & PERIODIC_MASK ;
  retardo_ms &= ~PERIODIC_MASK ; // Clear periodic bit

  // Find free slot or existing alarm with same ID (signed 32-bit integer)
  int32_t first_free_slot = -1;
  for (idx = 0; idx < svc_ALARMAS_MAX; idx++)
  {
    if (!alarmas[idx].active && first_free_slot == -1)
    {
      first_free_slot = idx;
    }
    else if (alarmas[idx].active && alarmas[idx].ID_evento == ID_evento)
    {
      break;
    }
  }

  // Check if we found a slot, if not signal overflow and halt
  if (first_free_slot == -1 && idx >= svc_ALARMAS_MAX)
  {
    drv_monitor_marcar(monitor_overflow);
    LOG_ERROR_F("Max number alarms reached while trying to program one for event %d", ID_evento);
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

  // If no active alarm for the current event was found, use the first free slot
  if (idx >= svc_ALARMAS_MAX)
  {
    idx = first_free_slot;
  }
  // Otherwise, reschedule the alarm @index `idx`

  Alarma newAlarma;
  newAlarma.retardo_ms = retardo_ms;
  newAlarma.ID_evento = ID_evento;
  newAlarma.auxData = auxData;
  newAlarma.active = (retardo_ms > 0);
  newAlarma.periodic = is_periodic;
  newAlarma.next_trigger = current_time + retardo_ms;

  if (retardo_ms > 0 && !alarmas[idx].active)
    num_alarmas_activas++;
  if (retardo_ms == 0 && alarmas[idx].active)
    num_alarmas_activas--;
  programacion_periodica_ideal = 0;

  alarmas[idx] = newAlarma;

  // Will return -1 if no alarms are enabled (might happen here
  // when disabling an alarm), 1 if next min trigger already passed
  // or the minimum amount of milliseconds remaining until the next
  // trigger occurs
  int32_t next = get_time_to_min_next_trigger(current_time);

  // No alarms are active, cancel periodic timer
  if (next == -1)
  {
    next = 0;
  }

  // KEY: do after setting next to 0 if next == -1
  if (next != next_trigger_timeout && next != -2)
  {
    drv_tiempo_periodico_ms(next, callback_encolar_global, ID_evento_global);
    next_trigger_timeout = next;
  }
  else if (next == -2)
  {
    svc_alarma_tratar(ev_T_PERIODICO, 0);
  }
  // else: drv_tiempo_periodico_ms will already be scheduled for the right next trigger timeout
}


void svc_alarma_tratar(uint32_t evento, uint32_t aux)
{
  uint32_t current_time = drv_tiempo_actual_ms();
  int8_t id_unica_alarma = -1;
  uint8_t tratado_no_periodica = 0;


  for (uint32_t i = 0; i < svc_ALARMAS_MAX; i++)
  {
    Alarma readAlarma = alarmas[i];
    if (readAlarma.active && current_time >= readAlarma.next_trigger)
    {
      // Enqueue the event directly into the FIFO
      if (readAlarma.periodic)
      {
        // Reschedule the periodic alarm
        id_unica_alarma = i;
        alarmas[i].next_trigger = current_time + readAlarma.retardo_ms;
      }
      else
      {
        // Disable single shot alarms
        tratado_no_periodica = 1;
        alarmas[i].active = 0;
        // One less active alarm
        num_alarmas_activas--;
      }

      callback_encolar_global(readAlarma.ID_evento, readAlarma.auxData);
    }
  }

  if (num_alarmas_activas > 0)
  {
    if (!tratado_no_periodica && num_alarmas_activas == 1 && !programacion_periodica_ideal)
    {
      // It means that we have a single alarm that is periodic and its not optimized in drv_tiempo_periodico
      programacion_periodica_ideal = 1;
      drv_tiempo_periodico_ms(0, callback_encolar_global, ID_evento_global);
      drv_tiempo_periodico_ms(alarmas[id_unica_alarma].retardo_ms, callback_encolar_global, ID_evento_global);
    }
    else
    {
      programacion_periodica_ideal = 0;
      int32_t next = get_time_to_min_next_trigger(current_time);
      if (next == -1)
      {
        next = 0;
      }
      
      if (next != next_trigger_timeout && next != -2)
      {
        drv_tiempo_periodico_ms(0, callback_encolar_global, ID_evento_global);
        drv_tiempo_periodico_ms(next, callback_encolar_global, ID_evento_global);
        next_trigger_timeout = next;
      }
      else if (next == -2)
      {
        // CRITICAL, SHOULD NEVER HAPPEN
        LOG_DEBUG("[SVC_ALARMA_TRATAR] Disabling periodic timer, next == -2");
        svc_log_procesar();
        svc_alarma_tratar(ev_T_PERIODICO, 0);
      }
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
  if (retardo_ms & PERIODIC_MASK)
  {
    drv_monitor_marcar(monitor_overflow);
    LOG_ERROR_F("Delay to big to process %d", retardo_ms);
    svc_log_procesar();
    while (1)
      ; // Halt system
  }

  // Encode: MSB = periodic flag, rest = delay
  return (periodico ? PERIODIC_MASK : 0) | retardo_ms;
}
