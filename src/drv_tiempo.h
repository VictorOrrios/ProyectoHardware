/**
 * @file drv_tiempo.h
 * @ingroup DRV
 * @brief Timer Driver Interface
 * @details Interface for the timer driver module. Provides hardware-independent
 *          timing services and types for time management.
 *
 * @defgroup DRV_TIMER Timer Driver
 * @ingroup DRV
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#ifndef DRV_TIEMPO
#define DRV_TIEMPO

#include <stdint.h>

/**
 * @brief Time types used by the application
 */
typedef uint64_t Tiempo_us_t;  // Microseconds
typedef uint32_t Tiempo_ms_t;  // Milliseconds

/**
 * @brief Initialize system timer and start counting
 */
void drv_tiempo_iniciar(void);

/**
 * @brief Get current time in microseconds
 * 
 * @return Tiempo_us_t Time since initialization in microseconds
 */
Tiempo_us_t drv_tiempo_actual_us(void);

/**
 * @brief Get current time in milliseconds
 * 
 * @return Tiempo_ms_t Time since initialization in milliseconds
 */
Tiempo_ms_t drv_tiempo_actual_ms(void);

/**
 * @brief Delay execution for specified time
 * 
 * @param ms Time to wait in milliseconds
 */
void drv_tiempo_esperar_ms(Tiempo_ms_t ms);

/**
 * @brief Wait until a specific time
 * 
 * @param ms Target time in milliseconds
 * @return Tiempo_ms_t Current time after wait
 */
Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t ms);

/**
 * @brief Configure periodic callback
 * 
 * @param periodo_ms Period between calls in milliseconds
 * @param funcion_encolar_evento Callback function
 * @param ID_evento Event ID to queue
 */
void drv_tiempo_periodico_ms(Tiempo_ms_t periodo_ms, void(*funcion_encolar_evento)(), uint32_t ID_evento);

/** @} */ // End of DRV_TIMER group

#endif
