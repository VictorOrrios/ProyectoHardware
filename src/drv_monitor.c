/**
 * @file drv_monitor.c
 * @ingroup DRV_MONITOR
 * @brief Debug Monitor Driver Implementation
 * @details Implementation of GPIO-based debug monitor functionality.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "hal_gpio.h"
#include "drv_monitor.h"
#include "board.h"

/** List of GPIO pins assigned to monitors */
#if MONITOR_NUMBER > 0
    static const uint8_t monitor_list[MONITOR_NUMBER] = MONITOR_LIST;
#endif

/**
 * @brief Initialize debug monitors
 * 
 * Configures GPIO pins as outputs and sets their initial state.
 * 
 * @return uint32_t Number of available monitors on the platform
 * @pre MONITOR_NUMBER and MONITOR_LIST must be defined in board.h
 */
uint32_t drv_monitor_iniciar(){
    #if MONITOR_NUMBER > 0
        // NOTE: pull resistors only have effect on input pins
        for (uint32_t i = 0; i < MONITOR_NUMBER; ++i) {
            hal_gpio_sentido(monitor_list[i], HAL_GPIO_PIN_DIR_OUTPUT);
            // By default, all pins will be off
            drv_monitor_desmarcar(i+1);
        }
    #endif     
    
    return MONITOR_NUMBER;  //defined in board_xxx.h for each board... 
}

/**
 * @brief Activate (mark) a specific monitor
 * 
 * @param id Monitor identifier (1 to MONITOR_NUMBER)
 * @pre Monitor must be initialized
 * @note Active state is defined by MONITOR_ACTIVE_STATE
 */
void drv_monitor_marcar(uint32_t id){
    #if MONITOR_NUMBER > 0
        if ((id <= MONITOR_NUMBER) && (id >0)) 
            hal_gpio_escribir(monitor_list[id-1], MONITOR_ACTIVE_STATE);
    #endif     
}

/**
 * @brief Deactivate (unmark) a specific monitor
 * 
 * @param id Monitor identifier (1 to MONITOR_NUMBER)
 * @pre Monitor must be initialized
 * @note Inactive state is the complement of MONITOR_ACTIVE_STATE
 */
void drv_monitor_desmarcar(uint32_t id){
    #if MONITOR_NUMBER > 0
        if ((id <= MONITOR_NUMBER) && (id >0)) 
            hal_gpio_escribir(monitor_list[id-1], ~MONITOR_ACTIVE_STATE);
    #endif     
}
