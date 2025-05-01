/**
 * @file hal_tiempo.h
 * @ingroup HAL
 * @brief Timer HAL Interface
 * @details Interface for the Timer Hardware Abstraction Layer.
 *          Provides functions to manage hardware timers independently
 *          of the specific platform.
 *
 * @defgroup HAL_TIMER Timer HAL
 * @ingroup HAL
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#ifndef HAL_TIEMPO
#define HAL_TIEMPO

#include <stdint.h>

/**
 * @brief Initialize and configure system tick counter
 * 
 * Configures and starts the hardware tick counter and returns
 * the conversion factor hal_ticks2us, which allows converting
 * from hardware ticks to microseconds.
 * 
 * @note The driver will use this to work with us and ms while
 *       converting to hardware ticks internally
 * 
 * @return uint32_t Conversion factor from ticks to microseconds
 */
uint32_t hal_tiempo_iniciar_tick(void);

/**
 * @brief Initialize tick counter with specific interval
 * 
 * @param intervalo Tick interval to configure
 */
void hal_tiempo_iniciar_tick2(uint64_t intervalo);

/**
 * @brief Get current tick count
 * 
 * @return uint64_t Total number of ticks since initialization
 */
uint64_t hal_tiempo_actual_tick(void);

/**
 * @brief Get current time in microseconds
 * 
 * @return uint64_t Total microseconds that has passed since initialization
 */
uint64_t hal_tiempo_actual_us(void);

/**
 * @brief Get current time in miliseconds
 * 
 * @return uint64_t Total miliseconds that has passed since initialization
 */
uint64_t hal_tiempo_actual_ms(void);

/**
 * @brief Configure periodic timer callback
 * 
 * @param periodo_en_tick Period in hardware ticks
 * @param funcion_callback Function to call periodically
 */
void hal_tiempo_reloj_periodico_tick(uint32_t periodo_en_tick, void(*funcion_callback)()); 

#endif
