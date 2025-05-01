/**
 * @file drv_uart.c
 * @ingroup DRV_UART
 * @brief UART Driver Implementation
 * @details Implementation of UART communication with monitor tracking and
 *          callback handling for transmission completion.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "drv_uart.h"
#include "hal_uart.h"
#include "drv_monitor.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/** @brief Monitor ID for UART activity tracking */
#define UART_MONITOR  4

/** @brief Function pointer for UART freed callback */
static void (*uart_freed_callback)();

/** @brief Event ID for UART freed notification */
static uint32_t uart_freed_event;

/**
 * @brief UART freed callback
 * 
 * @param event Event identifier
 * @param aux Auxiliary data (unused)
 */
void hal_uart_freed_callback() {
    uart_freed_callback(uart_freed_event, 0);
}

/**
 * @brief Initialize UART driver
 * 
 * @param callback Callback function to call when UART is freed
 * @param event Event identifier to be passed to the callback
 * 
 * Configures UART with standard parameters:
 * - 115200 baud
 * - 8 data bits
 * - No parity
 * - 1 stop bit
 */
void drv_uart_iniciar(void (*callback)(), uint32_t event) {
    uart_freed_callback = callback;
    uart_freed_event = event;
    hal_uart_iniciar(hal_uart_freed_callback);
}

/**
 * @brief Send a string through UART
 * 
 * @param str Pointer to the string buffer to send
 * @param len Length of the string buffer
 * @return 0 if transmission started, number of pending chars otherwise
 */
int32_t drv_uart_enviar_string(const char* str, uint32_t len) {
    if (!str || len == 0) return -2;
    
    drv_monitor_marcar(UART_MONITOR);
    
    if (hal_uart_enviar_buffer((uint8_t*)str, len)) {
        drv_monitor_desmarcar(UART_MONITOR);
        return 0;  // Transmission started successfully
    }
    
    drv_monitor_desmarcar(UART_MONITOR);
    return hal_uart_pendientes();  // Return actual pending bytes
}
