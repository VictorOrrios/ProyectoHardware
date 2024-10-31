/* *****************************************************************************
 * P.H.2024: Driver/Manejador de los temporizadores
 * suministra los servicios independientemente del hardware
 */
 
#ifndef DRV_TIEMPO
#define DRV_TIEMPO

#include <stdint.h>

/**
 * define los tipos de datos asociados a tiempo de uso en la app
 */
typedef uint64_t Tiempo_us_t;
typedef uint32_t Tiempo_ms_t;

/**
 * nicializa el reloj y empieza a contar
 */
void drv_tiempo_iniciar(void);

// @deprecated
// void drv_tiempo_iniciar2(uint64_t delay);

/**
 * tiempo desde que se inicio el temporizador en microsegundos
 */
Tiempo_us_t drv_tiempo_actual_us(void);

/**
 * tiempo desde que se inicio el temporizador en milisegundos
 */
Tiempo_ms_t drv_tiempo_actual_ms(void);

/**
 * retardo: esperar un cierto tiempo en milisegundos
 */
void drv_tiempo_esperar_ms(Tiempo_ms_t ms);

/**
 * esperar hasta un determinado tiempo (en ms), devuelve el tiempo actual
 */
Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t ms);

/**
 * @brief Configura una llamada periódica a una función callback
 * 
 * @param periodo_ms Periodo en milisegundos entre llamadas
 * @param funcion_encolar_evento Función a llamar periódicamente
 * @param ID_evento ID del evento a encolar
 */
void drv_tiempo_periodico_ms(Tiempo_ms_t periodo_ms, void(*funcion_encolar_evento)(), uint32_t ID_evento);

#endif
