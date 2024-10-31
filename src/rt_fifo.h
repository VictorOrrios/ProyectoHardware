/* *****************************************************************************
 * P.H.2024: Driver/Manejador de la cola de eventos
 * 
 * Este módulo implementa una cola FIFO (First In First Out) para la gestión
 * de eventos en tiempo real. Permite encolar y procesar eventos de manera
 * ordenada, manteniendo el orden temporal de llegada.
 */

#ifndef RT_FIFO
#define RT_FIFO

#include "hal_gpio.h"
#include "rt_evento_t.h"
#include "drv_tiempo.h"
#include <stdint.h>

/**
 * @brief Estructura que define un evento en el sistema
 * 
 * Cada evento contiene un identificador, datos auxiliares y una marca temporal
 * que indica cuándo fue encolado el evento.
 */
typedef struct{
    EVENTO_T ID_EVENTO; ///< Identificador del tipo de evento
    uint32_t auxData;   ///< Datos auxiliares específicos del evento
    Tiempo_us_t TS;     ///< Marca temporal en microsegundos
} EVENTO;

/**
 * @brief Tamaño máximo de la cola de eventos
 * 
 * Define cuántos eventos pueden estar pendientes simultáneamente.
 * @warning Debe ser una potencia de 2 para optimizar las operaciones módulo
 */
#define EVENT_QUEUE_SIZE 64

/**
 * @brief Inicializa la cola de eventos
 * 
 * @param pin_monitor_overflow Pin GPIO que se activará en caso de desbordamiento
 * @note El pin debe estar previamente configurado como salida
 */
void rt_FIFO_inicializar(HAL_GPIO_PIN_T pin_monitor_overflow);

/**
 * @brief Encola un nuevo evento en la cola FIFO
 * 
 * @param ID_evento Identificador del evento a encolar
 * @param auxData Datos auxiliares asociados al evento
 * 
 * @warning Si la cola está llena, se marca el pin de overflow y el sistema
 * entra en un bucle infinito como medida de seguridad
 */
void rt_FIFO_encolar(uint32_t ID_evento, uint32_t auxData);

/**
 * @brief Extrae el siguiente evento de la cola FIFO
 * 
 * @param[out] ID_evento Puntero donde se almacenará el ID del evento extraído
 * @param[out] auxData Puntero donde se almacenarán los datos auxiliares
 * @param[out] TS Puntero donde se almacenará la marca temporal
 * 
 * @return Número de eventos pendientes en la cola tras la extracción
 *         0 si la cola está vacía
 */
uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t *auxData, Tiempo_us_t *TS);

/**
 * @brief Obtiene estadísticas sobre los eventos procesados
 * 
 * @param ID_evento Tipo de evento del que se quieren obtener estadísticas
 * 
 * @return Para ev_VOID: suma total de todos los eventos encolados
 *         Para otros tipos: número total de veces que se ha encolado ese tipo
 */
uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento);

#endif
