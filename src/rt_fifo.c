/**
 * @file rt_fifo.c
 * @ingroup RT_FIFO
 * @brief Real-Time FIFO Queue Implementation
 * @details Implements the FIFO queue operations defined in rt_fifo.h using
 *          a circular buffer for efficient memory usage.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "rt_fifo.h"
#include "drv_monitor.h"
#include "drv_sc.h"
#include "svc_log.h"
#include "svc_stats.h"

/**
 * @brief Maximum size of the event queue
 * 
 * Defines how many events can be pending simultaneously.
 * @warning Must be a power of 2 to optimize modulo operations
 */
#define EVENT_QUEUE_SIZE 64

/**
 * @brief Structure that defines an event in the system
 * 
 * Each event contains an identifier, auxiliary data, and a timestamp
 * indicating when the event was queued.
 */
typedef struct{
    EVENTO_T ID_EVENTO; ///< Event type identifier
    uint32_t auxData;   ///< Event-specific auxiliary data
    Tiempo_us_t TS;     ///< Timestamp in microseconds
} EVENTO;

/** @brief Monitor pin for queue overflow detection */
static HAL_GPIO_PIN_T pin_monitor_overflow_global;

/** @brief Type definition for queue indices */
typedef uint32_t indice_cola_t;

/** @brief Circular buffer for event storage */
static volatile EVENTO fifo_task_queue[EVENT_QUEUE_SIZE];

/** @brief Statistics counter for each event type */
static volatile uint32_t total_times_queued_event[EVENT_TYPES];

static volatile indice_cola_t siguiente_indice_encolar = 0;
static volatile indice_cola_t siguiente_indice_procesar = 0;
static volatile uint32_t tamagno = 0;

/**
 * @brief Initialize the event queue
 * 
 * @param pin_monitor_overflow GPIO pin that will be activated in case of overflow
 * @note The pin must be previously configured as output
 */
void rt_FIFO_inicializar(HAL_GPIO_PIN_T pin_monitor_overflow) {
	pin_monitor_overflow_global = pin_monitor_overflow;
}

/**
 * @brief Queue a new event in the FIFO queue
 * 
 * @param ID_evento Event identifier to queue
 * @param auxData Auxiliary data associated with the event
 * 
 * @warning If the queue is full, the overflow pin is marked and the system
 * enters an infinite loop as a safety measure
 */
void rt_FIFO_encolar(uint32_t ID_evento, uint32_t auxData) {
	if (ID_evento >= EVENT_TYPES) return;

	if (tamagno >= EVENT_QUEUE_SIZE) {
		// Overflow detected
		drv_monitor_marcar(pin_monitor_overflow_global);
		LOG_ERROR("Max number of events in the FIFO queue reached");
		svc_log_procesar();
		while (1); // Infinite loop to indicate error
	} else {
		// Insert event into the queue
		EVENTO newEvento;
		newEvento.ID_EVENTO = (EVENTO_T)ID_evento;
		newEvento.auxData = auxData;
		newEvento.TS = drv_tiempo_actual_us();

		drv_sc_entrar(); // Start CS
		svc_stats_fifo_start();	// Record queued event
		fifo_task_queue[siguiente_indice_encolar] = newEvento;
		siguiente_indice_encolar = (siguiente_indice_encolar+1) % EVENT_QUEUE_SIZE;
		total_times_queued_event[ID_evento]++;
		tamagno++;
		drv_sc_salir(); // End CS
	}
}

/**
 * @brief Extract the next event from the FIFO queue
 * 
 * @param[out] ID_evento Pointer where the extracted event ID will be stored
 * @param[out] auxData Pointer where the auxiliary data will be stored
 * @param[out] TS Pointer where the timestamp will be stored
 * 
 * @return Number of pending events in the queue after extraction
 *         0 if the queue is empty
 */
uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t *auxData, Tiempo_us_t *TS) {
	if (tamagno <= 0) {
		return 0; // Queue is empty
	}
	
	drv_sc_entrar(); // Start CS
	EVENTO readEvento = fifo_task_queue[siguiente_indice_procesar];
	siguiente_indice_procesar = (siguiente_indice_procesar + 1) % EVENT_QUEUE_SIZE;
	uint32_t prevTamagno = tamagno;
	tamagno--;
	svc_stats_fifo_end();
	drv_sc_salir(); // End CS
	
	*ID_evento = readEvento.ID_EVENTO;
	*auxData = readEvento.auxData;
	*TS = readEvento.TS;
	
	return prevTamagno;
}

/**
 * @brief Get statistics about processed events
 * 
 * @param ID_evento Event type to get statistics for
 * 
 * @return For ev_VOID: total sum of all queued events
 *         For other types: total number of times that type has been queued
 */
uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento) {
	if (ID_evento == ev_VOID) {
		// Return the sum of all events ever enqueued
		uint32_t total = 0;

		drv_sc_entrar(); // Start CS
		for (int i = 0; i < EVENT_TYPES; i++) {
			total += total_times_queued_event[i];
		}
		drv_sc_salir(); // End CS

		return total;
	} else if (ID_evento < EVENT_TYPES) {
		drv_sc_entrar(); // Start CS
		uint32_t total_times_queued = total_times_queued_event[ID_evento];
		drv_sc_salir(); // End CS
		
		return total_times_queued;
	}
	return 0;
}
