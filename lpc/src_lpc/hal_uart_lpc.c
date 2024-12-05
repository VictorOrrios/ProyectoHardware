/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_uart_lpc.c - UART Hardware Abstraction Layer for LPC2105
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
 *   Implementation of the UART Hardware Abstraction Layer for the LPC2105
 *   microcontroller. This module handles UART initialization, configuration,
 *   and basic communication functions. It supports UART1 with interrupt
 *   capabilities.
 * *****************************************************************************/

#include "hal_uart.h"
#include <LPC210x.H>

// UART Control Registers
#define UART_ENABLE      0x01
#define UART_DATA_LEN_8  0x03
#define UART_STOP_BIT_1  0x00
#define UART_PARITY_NONE 0x00
#define UART_FIFO_EN     0x01
#define UART_BAUDRATE    115200

// VIC Vector Control Register bits
#define VIC_UART1_CHANNEL 7

/**
 * @brief UART1 Interrupt Service Routine
 * 
 * Handles UART1 interrupts and clears the interrupt flag
 */
void UART1_IRQHandler(void) __irq {
    // Clear interrupt
    VICVectAddr = 0;
} 

/**
 * @brief Initialize UART1 interface
 * 
 * Configures UART1 with the following settings:
 * - 115200 baud rate
 * - 8 data bits
 * - No parity
 * - 1 stop bit
 * - FIFO enabled
 */
void hal_uart_iniciar(void) {
    // 1. Setup PIN functions
    PINSEL0 |= 0x00050000;    // Enable RxD1 and TxD1 pins
    
    // 2. Setup UART Parameters
    U1LCR = 0x83;             // 8 bits, no Parity, 1 Stop bit, DLAB=1
    
    // Configure baudrate (assuming PCLK = 60MHz)
    uint32_t pclk = 60000000;
    uint32_t divisor = pclk / (16 * UART_BAUDRATE);
    U1DLL = divisor & 0xFF;
    U1DLM = (divisor >> 8) & 0xFF;
    
    U1LCR = 0x03;             // DLAB = 0
    U1FCR = 0x07;             // Enable and reset TX and RX FIFO
    
    // 3. Setup and enable UART interrupt in VIC
    VICVectAddr7 = (uint32_t)UART1_IRQHandler;
    VICVectCntl7 = 0x20 | VIC_UART1_CHANNEL;
    VICIntEnable |= (1 << VIC_UART1_CHANNEL);
}

/**
 * @brief Send a byte through UART1
 * 
 * Waits until the transmitter is ready and then sends the byte
 * 
 * @param byte The byte to send
 */
void hal_uart_enviar_byte(uint8_t byte) {
    while (!(U1LSR & 0x20)); // Wait until UART is ready to transmit
    U1THR = byte;            // Send byte
}
