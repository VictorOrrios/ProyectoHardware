/**
 * @file scheduler.h
 * @brief Planificador basado en eventos para la gestión de eventos del sistema y consumo energético
 * 
 * Este módulo implementa un planificador simple basado en eventos que procesa eventos desde
 * una cola FIFO y gestiona el consumo energético del sistema. Cuando no hay eventos presentes,
 * el procesador entra en un estado de espera de bajo consumo para conservar energía.
 * 
 * @author P.H.
 * @date 2024-10-30
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "rt_fifo.h"
#include "drv_leds.h"
#include "drv_consumo.h"

/**
 * @brief Ejecuta el planificador basado en eventos.
 * 
 * Inicializa la cola de eventos y configura un temporizador periódico para encolar eventos.
 * Comprueba continuamente si hay eventos para procesar, conmutando un LED si se detecta
 * un evento periódico. Si no hay eventos presentes, pone el procesador en un estado
 * de espera de bajo consumo.
 */
void scheduler_run(void);

#endif /* SCHEDULER_H */
