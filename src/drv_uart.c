#include "drv_uart.h"
#include "hal_uart.h"
#include "drv_monitor.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/** Monitor ID for UART activity tracking */
#define UART_MONITOR  4

/**
 * @brief Initialize UART driver
 * 
 * Configures UART with standard parameters:
 * - 115200 baud
 * - 8 data bits
 * - No parity
 * - 1 stop bit
 */
void drv_uart_iniciar(void) {
    hal_uart_iniciar();
}

/**
 * @brief Send a string through UART
 * 
 * @param str Null-terminated string to send
 * @note Uses monitor to track UART activity
 */
void drv_uart_enviar_string(const char* str) {
    if (!str) return;
    
    drv_monitor_marcar(UART_MONITOR);
    
    while (*str) {
        // Send character
        hal_uart_enviar_byte(*str++);
    }
    
    drv_monitor_desmarcar(UART_MONITOR);
}

/**
 * @brief Send a formatted string with variable arguments
 * 
 * @param format Format string
 * @param ... Variable arguments
 */
void drv_uart_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    drv_uart_enviar_string(buffer);
} 
