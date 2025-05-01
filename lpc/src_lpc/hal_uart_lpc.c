/**
 * @file hal_uart_lpc.c
 * @ingroup HAL_LPC
 * @brief UART HAL implementation for LPC2105
 * @details Implementation of the UART Hardware Abstraction Layer for the LPC2105
 *          microcontroller. This module handles UART initialization, configuration,
 *          and basic communication functions. It supports UART1 with interrupt
 *          capabilities.
 *
 * @defgroup HAL_LPC_UART UART Communication
 * @ingroup HAL_LPC
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "hal_uart.h"
#include <LPC210x.H>

// UART Control Registers
#define UART_LSR_THRE 0x20 // Transmit Hold Register Empty bit
#define UART_IIR_THRE 0x02 // THRE interrupt identification

// VIC Vector Control Register bits
#define VIC_UART1_CHANNEL 7

// Non-blocking implementation variables
static volatile const uint8_t *tx_buffer = 0;
static volatile uint32_t tx_length = 0;
static volatile uint32_t tx_index = 0;
static void (*uart_freed_callback)();

/**
 * @brief UART1 Interrupt Service Routine
 *
 * Handles UART1 interrupts for transmit
 */
void uart1_ISR(void) __irq
{
    uint32_t iir = U1IIR & 0x0F; // Get interrupt cause

    // Check if TX is complete (THRE interrupt)
    if (iir == UART_IIR_THRE)
    {
        if (tx_buffer && tx_index < tx_length)
        {
            U1THR = tx_buffer[tx_index++];
        }
        else
        {
            // Transmission complete
            tx_buffer = 0;
            tx_index = 0;
            uart_freed_callback();
        }
    }

    VICVectAddr = 0; // Acknowledge Interrupt

    // DONE: move inside if clause (otherwise, it
    // triggers every time a byte is sent through the UART)
    // uart_freed_callback();
}

void hal_uart_iniciar(void (*callback)(void))
{

    // Set UART freed callback
    uart_freed_callback = callback;

    // Setup PIN functions
    PINSEL0 |= 0x00050000; // Enable RxD1 and TxD1 pins

    // Setup UART Parameters (115200 8N1)
    U1LCR = 0x83; // 8 bits, no Parity, 1 Stop bit, DLAB=1

    // Calculate baudrate divisor (PCLK = 60MHz)
    U1DLL = 32; // Integer part
    U1DLM = 0;  // High byte

    U1LCR = 0x03; // DLAB = 0
    U1FCR = 0x07; // Enable and reset TX and RX FIFO

    // Non-blocking mode setup
    U1IER = 0x02; // Enable THRE interrupt

    // Setup UART1 interrupt
    VICVectAddr7 = (uint32_t)uart1_ISR;
    VICVectCntl7 = 0x20 | VIC_UART1_CHANNEL;
    VICIntEnable = 1 << VIC_UART1_CHANNEL;
}

/**
 * @brief Send a buffer through UART (non-blocking)
 * @param buffer Buffer to send
 * @param len Length of the buffer
 * @return 1 if transmission started, 0 if busy
 */
uint8_t hal_uart_enviar_buffer(const uint8_t *buffer, uint32_t len)
{
    // Non-blocking implementation
    if (tx_buffer != 0)
    {
        return 0;
    }

    tx_buffer = buffer;
    tx_length = len;
    tx_index = 0;

    if (len > 0)
    {
        U1THR = buffer[tx_index++];
    }

    return 1;
}

/**
 * @brief Get number of bytes pending transmission
 * @return Number of bytes still to be transmitted
 */
int32_t hal_uart_pendientes(void)
{
    if (!tx_buffer)
    {
        // No transmission in progress
        return 0;
    }
    else if (tx_index >= tx_length)
    {
        // Will usually happen if the whole buffer has been sent to the UART,
        // but uart1_ISR has not been triggered yet
        return -1;
    }
    else
    {
        // Transmission in progress, return remaining bytes
        return tx_length - tx_index;
    }
}

/** @} */ // End of HAL_LPC_UART group
