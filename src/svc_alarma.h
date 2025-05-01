/**
 * @file svc_alarma.h
 * @ingroup SVC_ALARM
 * @brief Alarm Service Interface
 * @details Provides a real-time alarm service that supports both one-shot and periodic
 *          alarms. Manages multiple concurrent alarms with overflow protection and
 *          efficient scheduling.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#ifndef SVC_ALARMA
#define SVC_ALARMA

#include <stdint.h>

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
 * @brief Encode a delay and periodic flag into a single 32-bit value
 * 
 * @param periodico true if alarm is periodic, false if one-shot
 * @param retardo_ms delay time in milliseconds (must be < 0x80000000)
 * @return uint32_t encoded value (MSB = periodic flag, rest = delay)
 */
uint32_t svc_alarma_codificar(uint8_t periodico, uint32_t retardo_ms);


#endif // SVC_ALARMA
