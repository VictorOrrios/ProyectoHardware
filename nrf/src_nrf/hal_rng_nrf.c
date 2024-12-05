/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_rng_nrf.c - Random Number Generator HAL for nRF52840
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
 *   Implementation of the Random Number Generator Hardware Abstraction Layer
 *   for the nRF52840 microcontroller. Uses the True Random Number Generator
 *   (TRNG) peripheral to generate random numbers.
 * *****************************************************************************/

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

