/* *****************************************************************************
 * Hardware Project 2024
 * 
 * svc_alarma.h - Alarm Service Interface
 * 
 * Description:
 *   Provides a real-time alarm service that supports both one-shot and periodic
 *   alarms. Manages multiple concurrent alarms with overflow protection and
 *   efficient scheduling.
 * 
 * Key Features:
 *   - Support for periodic and one-shot alarms
 *   - Dynamic alarm reprogramming
 *   - Overflow protection
 *   - Event-based notification system
 * *****************************************************************************/

#ifndef SVC_ALARMA
#define SVC_ALARMA

#include <stdint.h>
#include "rt_evento_t.h"

/**
 * @brief Maximum number of concurrent alarms
 * @done Set to 4
 */
#define svc_ALARMAS_MAX 4

/**
 * @brief Initialize the alarm service
 * @param overflow Monitor ID for overflow detection
 * @param f_callback Function to call when alarm triggers
 * @param ID_evento Event type to generate for alarm notifications
 */
void svc_alarma_iniciar(uint32_t overflow, void (*f_callback)(), uint32_t ID_evento);

/**
 * @brief Activate or reprogram an alarm
 * @param retardo_ms Delay in milliseconds (MSB indicates periodic/one-shot)
 * @param ID_evento Event ID for this alarm
 * @param auxData Additional data to pass with the event
 */
void svc_alarma_activar(uint32_t retardo_ms, uint32_t ID_evento, uint32_t auxData);

/**
 * @brief Process alarm events
 * @param evento Event type
 * @param aux Additional event data
 */
void svc_alarma_tratar(uint32_t evento, uint32_t aux);

/**
 * @brief Encode a delay and periodic flag into a single 32-bit value
 * 
 * @param periodico true if alarm is periodic, false if one-shot
 * @param retardo_ms delay time in milliseconds (must be < 0x80000000)
 * @return uint32_t encoded value (MSB = periodic flag, rest = delay)
 */
uint32_t svc_alarma_codificar(uint8_t periodico, uint32_t retardo_ms);

#endif // SVC_ALARMA
