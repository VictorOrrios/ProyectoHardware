/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_uart.h - UART HAL Interface
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
 *   Interface for the UART Hardware Abstraction Layer. Provides basic
 *   functions to manage serial communication independently of the
 *   specific platform.
 * *****************************************************************************/

#ifndef HAL_UART
#define HAL_UART

#include <stdint.h>

/**
 * @brief Initialize UART with specified baud rate
 * @param baudrate Baud rate for UART communication
 */
void hal_uart_iniciar(void);

/**
 * @brief Send a single byte through UART
 * @param byte Byte to send
 */
void hal_uart_enviar_byte(uint8_t byte);

/**
 * @brief Check if UART transmitter is ready to send
 * @return 1 if ready, 0 if busy
 */
uint32_t hal_uart_listo_tx(void);

#endif 
