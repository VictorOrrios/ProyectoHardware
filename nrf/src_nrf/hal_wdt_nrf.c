/**
 * @file hal_wdt_nrf.c
 * @ingroup HAL_NRF
 * @brief Watchdog Timer HAL implementation for nRF52840
 * @details Implementation of the Watchdog Timer Hardware Abstraction Layer for
 *          the nRF52840 microcontroller. This module manages watchdog configuration
 *          and handling, supporting interrupt-based watchdog operation.
 *
 * @defgroup HAL_NRF_WDT Watchdog Timer
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

#include "hal_wdt.h"
#include "nrf.h"

static void (*wdt_callback)(void) = 0;

/**
 * @brief Initialize the watchdog timer
 * 
 * @param timeout_ms Timeout period in milliseconds
 */
void hal_wdt_iniciar(uint32_t timeout_ms) {
    // Convert ms to 32768Hz ticks
    uint32_t ticks = (timeout_ms * 32768) / 1000;
    
    // Configure WDT
    NRF_WDT->CONFIG = 
        (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos) |     // Keep running while CPU sleeps
        (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos);      // Pause while debugging
    
    NRF_WDT->CRV = ticks;
    NRF_WDT->RREN = WDT_RREN_RR0_Msk;  // Enable reload register 0
    
	// See https://devzone.nordicsemi.com/f/nordic-q-a/769/how-does-the-watchdog-work
    // Setup warning interrupt
    NRF_WDT->INTENSET = WDT_INTENSET_TIMEOUT_Msk;
    NVIC_SetPriority(WDT_IRQn, 0); // Highest priority
    NVIC_EnableIRQ(WDT_IRQn);
    
    // Start WDT
    NRF_WDT->TASKS_START = 1;
}

/**
 * @brief Feed the watchdog timer
 * 
 * Prevents the watchdog from triggering by feeding the required sequence
 */
void hal_wdt_feed(void) {
    // NOTE: don't use critical section => yes for LPC
    NRF_WDT->RR[0] = WDT_RR_RR_Reload;  // Magic value: 0x6E524635
}

/**
 * @brief Register a callback function for watchdog events
 * 
 * @param callback Function to call when watchdog triggers
 */
void hal_wdt_registrar_callback(void (*callback)(void)) {
    wdt_callback = callback;
}

/**
 * @brief Watchdog Timer Interrupt Handler
 */
void WDT_IRQHandler(void) {
    if (NRF_WDT->EVENTS_TIMEOUT) {
        NRF_WDT->EVENTS_TIMEOUT = 0;
        
        // Add memory barrier to ensure event is cleared
        volatile uint32_t dummy = NRF_WDT->EVENTS_TIMEOUT;
        (void)dummy;
        
        if (wdt_callback) {
            wdt_callback();
        }
    }
}

/** @} */ // End of HAL_NRF_WDT group
