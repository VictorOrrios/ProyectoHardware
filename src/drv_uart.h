/* *****************************************************************************
 * Hardware Project 2024
 * 
 * drv_uart.h - UART Driver Interface
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
 *   Interface for the UART driver module. Provides functions for serial
 *   communication including string transmission and formatted output.
 * *****************************************************************************/

#ifndef DRV_UART
#define DRV_UART

#include <stdint.h>

/**
 * @brief Initialize UART driver
 * 
 * Configures UART with standard parameters:
 * - 115200 baud
 * - 8 data bits
 * - No parity
 * - 1 stop bit
 */
void drv_uart_iniciar(void);

/**
 * @brief Send a string through UART
 * 
 * @param str Null-terminated string to send
 */
void drv_uart_enviar_string(const char* str);

/**
 * @brief Send formatted string (printf style)
 * 
 * @param format Format string
 * @param ... Variable arguments
 */
void drv_uart_printf(const char* format, ...);

#endif 
