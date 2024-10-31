/* *****************************************************************************
 * P.H.2024: Driver/Manejador de la cola de eventos
 * 
 * Este módulo define los tipos de eventos que el sistema puede manejar.
 * Cada evento representa una acción o suceso específico que debe ser
 * procesado por el sistema en tiempo real.
 */

#ifndef RT_EVENTO
#define RT_EVENTO

/**
 * @brief Enumeración de los tipos de eventos soportados por el sistema
 * 
 * @note Los valores numéricos son importantes y no deben modificarse
 * ya que pueden ser utilizados como índices en arrays.
 */
typedef enum {
    ev_VOID = 0,        ///< Evento nulo o sin tipo
    ev_T_PERIODICO = 1, ///< Evento de temporización periódica
    ev_PULSAR_BOTON = 2,///< Evento de pulsación de botón
} EVENTO_T; // mapea a uint32_t

/**
 * @brief Número total de tipos de eventos definidos en el sistema
 * 
 * Este valor debe actualizarse si se añaden nuevos tipos de eventos
 * en la enumeración EVENTO_T.
 */
#define EVENT_TYPES 3

#endif
