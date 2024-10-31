#include "rt_fifo.h"
#include "drv_monitor.h"

HAL_GPIO_PIN_T pin_monitor_overflow_global;

typedef uint32_t indice_cola_t; // Define the type for queue indices

static volatile EVENTO fifo_task_queue[EVENT_QUEUE_SIZE];
static volatile uint32_t total_times_queued_event[EVENT_TYPES];

static volatile indice_cola_t siguiente_indice_encolar = 0;
static volatile indice_cola_t siguiente_indice_procesar = 0;
static volatile uint32_t tamagno = 0;

/**
 * @brief Inicializa la cola de eventos
 * 
 * @param pin_monitor_overflow Pin GPIO que se activará en caso de desbordamiento
 * @note El pin debe estar previamente configurado como salida
 */
void rt_FIFO_inicializar(HAL_GPIO_PIN_T pin_monitor_overflow) {
	pin_monitor_overflow_global = pin_monitor_overflow;
}

/**
 * @brief Encola un nuevo evento en la cola FIFO
 * 
 * @param ID_evento Identificador del evento a encolar
 * @param auxData Datos auxiliares asociados al evento
 * 
 * @warning Si la cola está llena, se marca el pin de overflow y el sistema
 * entra en un bucle infinito como medida de seguridad
 */
void rt_FIFO_encolar(uint32_t ID_evento, uint32_t auxData) {
	if (ID_evento >= EVENT_TYPES) return;

	if (tamagno >= EVENT_QUEUE_SIZE) {
		// Overflow detected
		drv_monitor_marcar(pin_monitor_overflow_global);
		while (1); // Infinite loop to indicate error
	} else {
		// Insert event into the queue
		fifo_task_queue[siguiente_indice_encolar].ID_EVENTO = ID_evento;
		fifo_task_queue[siguiente_indice_encolar].auxData = auxData;
		fifo_task_queue[siguiente_indice_encolar].TS = drv_tiempo_actual_us();
		siguiente_indice_encolar = (siguiente_indice_encolar +1) % EVENT_QUEUE_SIZE;
		total_times_queued_event[ID_evento]++;
		tamagno++;
	}
}

/**
 * @brief Extrae el siguiente evento de la cola FIFO
 * 
 * @param[out] ID_evento Puntero donde se almacenará el ID del evento extraído
 * @param[out] auxData Puntero donde se almacenarán los datos auxiliares
 * @param[out] TS Puntero donde se almacenará la marca temporal
 * 
 * @return Número de eventos pendientes en la cola antes de la extracción
 *         0 si la cola está vacía
 */
uint8_t rt_FIFO_extraer(EVENTO_T *ID_evento, uint32_t *auxData, Tiempo_us_t *TS) {
	if (tamagno <= 0) {
		return 0; // Queue is empty
	}

	*ID_evento = fifo_task_queue[siguiente_indice_procesar].ID_EVENTO;
	*auxData = fifo_task_queue[siguiente_indice_procesar].auxData;
	*TS = fifo_task_queue[siguiente_indice_procesar].TS;
	siguiente_indice_procesar = (siguiente_indice_procesar + 1) % EVENT_QUEUE_SIZE;

	tamagno--;
	
	return tamagno +1;
}

/**
 * @brief Obtiene estadísticas sobre los eventos procesados
 * 
 * @param ID_evento Tipo de evento del que se quieren obtener estadísticas
 * 
 * @return Para ev_VOID: suma total de todos los eventos encolados
 *         Para otros tipos: número total de veces que se ha encolado ese tipo
 */
uint32_t rt_FIFO_estadisticas(EVENTO_T ID_evento) {
	// TODO: revise
	if (ID_evento == ev_VOID) {
		// Return the sum of all events ever enqueued
		uint32_t total = 0;
		for (int i = 0; i < EVENT_TYPES; i++) {
			total += total_times_queued_event[i];
		}
		return total;
	} else if (ID_evento < EVENT_TYPES) {
		return total_times_queued_event[ID_evento];
	}
	return 0;
}

/**
 * @brief Scheduler: toma eventos de la cola y los ejecuta. Si no hay más eventos, ENTRA EN MODO DORMIDO
 * 
 * @note NO usar nombres head y tail (índices), USAR NOMBRES MÁS DESCRIPTIVOS
 */
