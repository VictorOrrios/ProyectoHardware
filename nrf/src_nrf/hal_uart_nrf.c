/**
 * @file hal_uart_nrf.c
 * @ingroup HAL_NRF
 * @brief UART HAL implementation for nRF52840
 * @details Implementation of the UART Hardware Abstraction Layer for the nRF52840
 *          microcontroller. This module handles UART initialization and basic
 *          communication functions using the UARTE peripheral.
 *
 * @defgroup HAL_NRF_UART UART Communication
 * @ingroup HAL_NRF
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
#include "nrf.h"
#include "board.h"

#define PIN_TXD 6
#define PIN_RXD 8
#define UART_BAUDRATE UARTE_BAUDRATE_BAUDRATE_Baud115200

// Buffer for non-blocking transmission
static volatile const uint8_t *tx_buffer = 0;
static volatile uint32_t tx_length = 0;

// Callback for UART freed events
static void (*f_callback)() = 0;

/**
 * @brief UARTE interrupt handler
 *
 * Handles ENDTX events and marks transmission as complete
 */
void UARTE0_UART0_IRQHandler(void)
{
    if (NRF_UARTE0->EVENTS_ENDTX)
    {
        NRF_UARTE0->EVENTS_ENDTX = 0; // Clear event first
        NRF_UARTE0->TASKS_STOPTX = 1; // Stop TX to ensure clean state
        tx_buffer = 0;                // Mark transmission as complete
        if (f_callback)
        {
            f_callback();
        }
    }
}

/**
 * @brief Initialize UART interface
 *
 * Configures UART0 with the following settings:
 * - 115200 baud rate
 * - No hardware flow control
 * - No parity
 */
void hal_uart_iniciar(void (*callback)(void))
{

    // Set UART freed callback
    f_callback = callback;

    // Configure GPIO pins
    NRF_GPIO->PIN_CNF[PIN_TXD] =
        (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos) |
        (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
        (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
        (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
        (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);

    NRF_GPIO->PIN_CNF[PIN_RXD] =
        (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
        (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos) |
        (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
        (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos) |
        (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);

    // First disable UART to ensure clean configuration
    NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;

    // Clear any pending events
    NRF_UARTE0->EVENTS_ENDTX = 0;
    NRF_UARTE0->EVENTS_TXSTOPPED = 0;

    // Configure UART
    NRF_UARTE0->CONFIG =
        (UART_CONFIG_HWFC_Disabled << UART_CONFIG_HWFC_Pos) |
        (UARTE_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos);

    NRF_UARTE0->BAUDRATE = UART_BAUDRATE << UARTE_BAUDRATE_BAUDRATE_Pos; // Remove the shift, BAUDRATE already includes position
    NRF_UARTE0->PSEL.TXD = PIN_TXD;
    NRF_UARTE0->PSEL.RXD = PIN_RXD;

    // Configure interrupts
    NRF_UARTE0->INTENSET = UARTE_INTENSET_ENDTX_Msk; // Use mask instead of shift

    // Set interrupt priority and enable
    NVIC_SetPriority(UARTE0_UART0_IRQn, 7); // Set to lowest priority
    NVIC_EnableIRQ(UARTE0_UART0_IRQn);

    // Finally enable UART
    NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;
}

/**
 * @brief Send a buffer through UART (non-blocking)
 * @param buffer Buffer to send
 * @param len Length of the buffer
 * @return 1 if transmission started, 0 if busy
 */
uint8_t hal_uart_enviar_buffer(const uint8_t *buffer, uint32_t len)
{
    if (tx_buffer != 0)
    {
        return 0; // Previous transmission still ongoing
    }

    tx_buffer = buffer;
    tx_length = len;

    NRF_UARTE0->TXD.PTR = (uint32_t)buffer;
    NRF_UARTE0->TXD.MAXCNT = len;
    NRF_UARTE0->TASKS_STARTTX = 1;

    return 1;
}

/**
 * @brief Get number of bytes pending transmission
 * @return Number of bytes still to be transmitted:
 *         - Positive number: Number of bytes pending in current transmission
 *         - 0: No transmission in progress or transmission complete
 *         - -1: Error in transmission (e.g., AMOUNT overflow in NRF)
 * @note In blocking mode (LPC), always returns 0 since transmissions are synchronous
 */
int32_t hal_uart_pendientes(void)
{
    if (!tx_buffer)
    {
        // No transmission in progress
        return 0;
    }
    else if (NRF_UARTE0->TXD.AMOUNT >= tx_length)
    {
        // Will usually happen if the whole buffer has been sent to the UART,
        // but UARTE0_UART0_IRQHandler has not been triggered yet
        return -1;
    }
    else
    {
        // Transmission in progress, return remaining bytes
        return tx_length - NRF_UARTE0->TXD.AMOUNT;
    }
}

/** @} */ // End of HAL_NRF_UART group
