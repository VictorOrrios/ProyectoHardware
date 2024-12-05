/* *****************************************************************************
 * Hardware Project 2024
 * 
 * drv_tiempo.h - Timer Driver Interface
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
 *   Interface for the timer driver module. Provides hardware-independent
 *   timing services and types for time management.
 * *****************************************************************************/

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

#endif
