/**
 * @file hal_wdt_lpc.c
 * @ingroup HAL_LPC
 * @brief Watchdog Timer HAL implementation for LPC2105
 * @details Implementation of the Watchdog Timer Hardware Abstraction Layer for
 *          the LPC2105 microcontroller. This module manages watchdog configuration
 *          and handling, supporting interrupt-based watchdog operation.
 *
 * @defgroup HAL_LPC_WDT Watchdog Timer
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

#include "hal_wdt.h"
#include "board.h"
#include <LPC210x.H>

#ifndef PCLK
#define PCLK 16000000  // PCLK frequency in Hz
#endif

static void (*wdt_callback)(void) = 0;

/**
 * @brief Watchdog Timer Interrupt Handler
 * 
 * Calls the registered callback function and feeds the watchdog
 * to prevent system reset
 */
void WDT_IRQHandler(void) {
    if (wdt_callback) {
        wdt_callback();
    }
		
    WDFEED = 0xAA;   // Feed sequence
    WDFEED = 0x55;
		
		while(1);
}

/**
 * @brief Initialize the watchdog timer
 * 
 * @param timeout_ms Timeout period in milliseconds
 */
void hal_wdt_iniciar(uint32_t timeout_ms) {
    // Check for previous timeout
    if (WDMOD & 0x04) {
        WDMOD &= ~0x04;  // Clear timeout flag
    }

    // Configure timeout in WDTC
    WDTC = timeout_ms * (PCLK / (4 * 1000));

    __disable_irq();  // Disable global interrupts for configuration

    // Configure WDT interrupt as FIQ in VIC
    VICIntSelect |= (1 << 0);     // Set WDT as FIQ
    VICIntEnable |= (1 << 0);     // Enable WDT interrupt in VIC

    __enable_irq();  // Re-enable global interrupts

    // Configure WDT in reset mode
    WDMOD = 0b11;
		
		// Configure WDT in interrupt mode
    //WDMOD = 0b01;

    // Feed WDT to start timer
    hal_wdt_feed();
}

/**
 * @brief Feed the watchdog timer
 * 
 * Prevents the watchdog from triggering by feeding the required sequence
 */
void hal_wdt_feed(void) {
    __disable_irq();  // Disable all interrupts
    WDFEED = 0xAA;    // Feed sequence
    WDFEED = 0x55;
    __enable_irq();    // Re-enable interrupts
}

/**
 * @brief Register a callback function for watchdog events
 * 
 * @param callback Function to call when watchdog triggers
 */
void hal_wdt_registrar_callback(void (*callback)(void)) {
    wdt_callback = callback;
}

/** @} */ // End of HAL_LPC_WDT group
