/**
 * @file drv_tiempo.c
 * @ingroup DRV_TIMER
 * @brief Timer Driver Implementation
 * @details Implementation of system timing and periodic event management.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */
 
#include "drv_tiempo.h"
#include "hal_tiempo.h"

/** System tick to microsecond conversion factor */
uint64_t factorTickToUs;

/** Global variables for periodic event handling */
static uint32_t ID_evento_global;
static void(*drv_callback)();

/**
 * @brief Initialize system timer
 * 
 * Configures system timer and gets the conversion factor
 * between ticks and microseconds.
 */
void drv_tiempo_iniciar(void){
    factorTickToUs = hal_tiempo_iniciar_tick();
}



/**
 * @brief Get current time in microseconds
 * 
 * @return Time since initialization in microseconds
 * @pre Driver must be initialized
 */
Tiempo_us_t drv_tiempo_actual_us(void){
		return hal_tiempo_actual_us();
}

/**
 * @brief Get current time in milliseconds
 * 
 * @return Time since initialization in milliseconds
 * @pre Driver must be initialized
 */
Tiempo_ms_t drv_tiempo_actual_ms(void){
		return hal_tiempo_actual_ms();
}

/**
 * @brief Generate a delay for specified time
 * 
 * @param ms Time to wait in milliseconds
 * @pre Driver must be initialized
 */
void drv_tiempo_esperar_ms(Tiempo_ms_t ms){
    while(drv_tiempo_actual_ms() < ms);
}

/**
 * @brief Wait until a specific time
 * 
 * @param ms Target time in milliseconds
 * @return Current time after wait
 * @pre Driver must be initialized
 */
Tiempo_ms_t drv_tiempo_esperar_hasta_ms(Tiempo_ms_t ms){
    drv_tiempo_esperar_ms(ms);
    return drv_tiempo_actual_ms();
}

/**************************************************/
/* Internal helper functions */
/**************************************************/

/**
 * @brief Internal callback for handling periodic events
 */
void hal_callback_no_params(void) {
    drv_callback(ID_evento_global, drv_tiempo_actual_ms());
}

/**
 * @brief Convert milliseconds to system ticks
 * 
 * @param periodo_ms Period in milliseconds
 * @return Equivalent period in system ticks
 */
uint32_t periodo_en_tick(Tiempo_ms_t periodo_ms) {
    return (uint32_t)(periodo_ms * 1000 * factorTickToUs);
}

/**
 * @brief Configure periodic function call
 * 
 * @param periodo_ms Period between calls in milliseconds
 * @param funcion_encolar_evento Function to call periodically
 * @param ID_evento Event identifier to generate
 * @pre Driver must be initialized
 */
void drv_tiempo_periodico_ms(Tiempo_ms_t periodo_ms, void(*funcion_encolar_evento)(), uint32_t ID_evento){
    ID_evento_global = ID_evento;
    drv_callback = funcion_encolar_evento;
    hal_tiempo_reloj_periodico_tick(periodo_en_tick(periodo_ms), hal_callback_no_params);
}
