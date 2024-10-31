/**
 * @file drv_monitor.c
 * @brief Driver/Manejador de los monitores de depuración
 * 
 * Este módulo implementa la gestión de pines GPIO utilizados como monitores
 * para depuración. Permite marcar y desmarcar señales para seguimiento
 * temporal del programa.
 * 
 * @author P.H.2024
 * @date 2024
 * @version 1.0
 * @dependencies hal_gpio.h, board.h
 */

#include "hal_gpio.h"
#include "drv_monitor.h"
#include "board.h"

/** Lista de pines GPIO asignados a los monitores */
#if MONITOR_NUMBER > 0
    static const uint8_t monitor_list[MONITOR_NUMBER] = MONITOR_LIST;
#endif

/**
 * @brief Inicializa los monitores de depuración
 * 
 * Configura los pines GPIO como salidas y los marca inicialmente.
 * 
 * @return Número de monitores disponibles en la plataforma
 * @pre MONITOR_NUMBER y MONITOR_LIST deben estar definidos en board.h
 */
uint32_t drv_monitor_iniciar(){
    #if MONITOR_NUMBER > 0
        for (uint32_t i = 0; i < MONITOR_NUMBER; ++i) {
            hal_gpio_sentido(monitor_list[i], HAL_GPIO_PIN_DIR_OUTPUT);
            drv_monitor_desmarcar(i+1);
        }
    #endif     
    
    return MONITOR_NUMBER;  //definido en board_xxx.h en cada placa... 
}

/**
 * @brief Activa (marca) un monitor específico
 * 
 * @param id Identificador del monitor (1 a MONITOR_NUMBER)
 * @pre El monitor debe haber sido inicializado
 * @note El estado activo se define mediante MONITOR_ACTIVE_STATE
 */
void drv_monitor_marcar(uint32_t id){
    #if MONITOR_NUMBER > 0
        if ((id <= MONITOR_NUMBER) && (id >0)) 
            hal_gpio_escribir(monitor_list[id-1], MONITOR_ACTIVE_STATE);
    #endif     
}

/**
 * @brief Desactiva (desmarca) un monitor específico
 * 
 * @param id Identificador del monitor (1 a MONITOR_NUMBER)
 * @pre El monitor debe haber sido inicializado
 * @note El estado inactivo es el complemento de MONITOR_ACTIVE_STATE
 */
void drv_monitor_desmarcar(uint32_t id){
    #if MONITOR_NUMBER > 0
        if ((id <= MONITOR_NUMBER) && (id >0)) 
            hal_gpio_escribir(monitor_list[id-1], ~MONITOR_ACTIVE_STATE);
    #endif     
}
