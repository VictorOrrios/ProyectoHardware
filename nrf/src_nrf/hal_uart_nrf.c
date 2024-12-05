/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_uart_nrf.c - UART Hardware Abstraction Layer for nRF52840
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
 *   Implementation of the UART Hardware Abstraction Layer for the nRF52840
 *   microcontroller. This module handles UART initialization and basic
 *   communication functions using the UARTE peripheral.
 * *****************************************************************************/

#include "hal_uart.h"
#include "nrf.h"
#include "board.h"

#define PIN_TXD 6
#define PIN_RXD 8
#define UART_BAUDRATE UARTE_BAUDRATE_BAUDRATE_Baud115200

/**
 * @brief Initialize UART interface
 * 
 * Configures UART0 with the following settings:
 * - 115200 baud rate
 * - No hardware flow control
 * - No parity
 */
void hal_uart_iniciar() {
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

    // Configure UART
    NRF_UARTE0->CONFIG = 
        (UART_CONFIG_HWFC_Disabled << UART_CONFIG_HWFC_Pos) |
        (UARTE_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos);
    
    NRF_UARTE0->BAUDRATE = UART_BAUDRATE << UARTE_BAUDRATE_BAUDRATE_Pos;
    NRF_UARTE0->PSEL.TXD = PIN_TXD;
    NRF_UARTE0->PSEL.RXD = PIN_RXD;
    NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;
}

/**
 * @brief Send a byte through UART
 * 
 * @param byte The byte to send
 */
void hal_uart_enviar_byte(uint8_t byte) {

    // NOTE: See https://git.riot-os.org/RIOT-OS/RIOT/commit/250b7cbbbf6ffd5fab619ac81e6c02928be0f12a.diff > uart_write
    NRF_UARTE0->TXD.MAXCNT = 1;
    NRF_UARTE0->TXD.PTR = (uint32_t)&byte;
    NRF_UARTE0->TASKS_STARTTX = 1;
    
    while (NRF_UARTE0->EVENTS_ENDTX == 0) {}
    NRF_UARTE0->EVENTS_ENDTX = 0;
}
