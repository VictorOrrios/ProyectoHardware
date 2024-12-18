/**
 * @file drv_uart.h
 * @ingroup DRV
 * @brief UART Driver Interface
 * @details Interface for the UART driver module. Provides functions for serial
 *          communication including string transmission and formatted output.
 *
 * @defgroup DRV_UART UART Driver
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

#ifndef DRV_UART
#define DRV_UART

#include <stdint.h>

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
void drv_uart_iniciar(void (*callback)(), uint32_t event);

/**
 * @brief Send a string through UART
 * 
 * @param str Pointer to the string buffer to send
 * @param len Length of the string buffer
 * @return 0 if transmission started, number of pending chars otherwise
 */
int32_t drv_uart_enviar_string(const char* str, uint32_t len);

/** @} */ // End of DRV_UART group

#endif 
