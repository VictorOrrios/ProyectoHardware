/* *****************************************************************************
 * Hardware Project 2024
 * 
 * rt_evento_t.h - Event Types Definition
 * 
 * Authors:
 *   - Víctor Orrios Barón (NIA: 840994)
 *   - José Miguel Quílez Vergara (NIA: 873499)
 * 
 * EINA - University of Zaragoza
 * Computer Science and Engineering
 * Course: 3rd year, 1st semester
 * 
 * Date: 02/12/2024
 * 
 * Description:
 *   Defines the event types that can be handled by the real-time system.
 *   Each event represents a specific action or occurrence that needs to
 *   be processed by the system. The numeric values are significant as
 *   they may be used as array indices.
 * *****************************************************************************/

#ifndef RT_EVENTO
#define RT_EVENTO

/**
 * @brief Enumeración de los tipos de eventos soportados por el sistema
 * 
 * @note Los valores numéricos son importantes y no deben modificarse
 * ya que pueden ser utilizados como índices en arrays.
 */
typedef enum {
    ev_VOID = 0,        			///< Evento nulo o sin tipo
    ev_T_PERIODICO = 1, 			///< Evento de temporización periódica
    ev_PULSAR_BOTON = 2,			///< Evento de pulsación de botón
    ev_INACTIVIDAD = 3,  			///< Evento de inactivdad de usuario
    ev_RETARDO = 4,     			///< Evento de retardo
    ev_BLINK = 5,       			///< Evento para el parpadeo del LED
    ev_WDT_CHECK = 6,    			///< Evento para comprobación del watchdog
		ev_COUNTER_STRIKE = 7, 		///< Evento para jugar al bit-counter-strike
		ev_TEST = 8,							///< Evento generico que pueden utilizar todos los modulos de test
		ev_SIMON_GENERICO = 9,    ///< Evento de simon dice para una ocasuion generica
		ev_FEED_WDT = 10,					///< Evento de feed para el watchdog
		ev_DOBLE_BOTON_PULSADO = 11,
    ev_ENABLE_BUTTON_REBOUNCE_ALARM = 12,
    ev_TEST_CS_MAIN = 13,
    ev_DUMP_ALL_STATS = 14
} EVENTO_T; // mapea a uint32_t

/**
 * @brief Número total de tipos de eventos definidos en el sistema
 * 
 * Este valor debe actualizarse si se añaden nuevos tipos de eventos
 * en la enumeración EVENTO_T.
 */
#define EVENT_TYPES 15
#define ev_NUM_EV_USUARIO 1
#define ev_USUARIO {ev_PULSAR_BOTON}

#endif
