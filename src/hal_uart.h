/**
 * @file hal_uart.h
 * @ingroup HAL
 * @brief UART HAL Interface
 * @details Interface for the UART Hardware Abstraction Layer.
 *          Provides basic functions to manage serial communication
 *          independently of the specific platform.
 *
 * @defgroup HAL_UART UART HAL
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

#ifndef HAL_UART
#define HAL_UART

#include <stdint.h>

/**
 * @brief Initialize UART with specified baud rate
 * @param baudrate Baud rate for UART communication
 */
void hal_uart_iniciar(void (*callback)(void));

/**
 * @brief Send a buffer through UART (non-blocking)
 * @param buffer Buffer to send
 * @param len Length of the buffer
 * @return 1 if transmission started successfully, 0 if busy
 */
uint8_t hal_uart_enviar_buffer(const uint8_t* buffer, uint32_t len);

/**
 * @brief Get number of bytes pending transmission
 * @return Number of bytes still to be transmitted:
 *         - Positive number: Number of bytes pending
 *         - 0: No transmission in progress
 *         - -1: Error in transmission
 */
int32_t hal_uart_pendientes(void);

#endif 
