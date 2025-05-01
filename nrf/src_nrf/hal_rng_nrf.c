/**
 * @file hal_rng_nrf.c
 * @ingroup HAL_NRF
 * @brief Random Number Generator HAL implementation for nRF52840
 * @details Implementation of the Random Number Generator Hardware Abstraction Layer
 *          for the nRF52840 microcontroller. Uses the True Random Number Generator
 *          (TRNG) peripheral to generate random numbers.
 *
 * @defgroup HAL_NRF_RNG Random Number Generator
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

#include "hal_rng.h"
#include "nrf.h"

/**
 * @brief Initialize the random number generator
 * 
 * @param seed Initial seed value (unused as hardware RNG is used)
 */
void hal_rng_init(uint64_t seed) {
    // Enable TRNG
    NRF_RNG->TASKS_START = 1;
    // Enable bias correction
    NRF_RNG->CONFIG = 1;
    NRF_RNG->SHORTS = 0;
    
}

/**
 * @brief Generate a random byte
 * 
 * @param byte Pointer to store the generated random byte
 * @return uint8_t 1 if successful, 0 if failed
 */
uint8_t hal_rng_get_byte(uint8_t* byte) {
    if (!byte ) {
        return 0;
    }

    // Clear the event flag
    NRF_RNG->EVENTS_VALRDY = 0;
    
    // Wait for data ready event
    while (!NRF_RNG->EVENTS_VALRDY) {
        // Could add timeout here if needed
    }
    
    *byte = (uint8_t)NRF_RNG->VALUE;
    return 1;
}

/**
 * @brief Stop the random number generator
 */
void hal_rng_stop(void) {
    NRF_RNG->TASKS_STOP = 1;
}

/** @} */ // End of HAL_NRF_RNG group

