/**
 * @file hal_rng_lpc.c
 * @ingroup HAL_LPC
 * @brief Random Number Generator HAL implementation for LPC2105
 * @details Implementation of the Random Number Generator Hardware Abstraction Layer
 *          for the LPC2105 microcontroller. This module provides basic random number
 *          generation functionality using the standard C library's rand() function.
 *
 * @defgroup HAL_LPC_RNG Random Number Generator
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

#include "hal_rng.h"
#include <LPC210x.H>
#include <stdlib.h>

/**
 * @brief Initialize the random number generator
 * 
 * @param seed Initial seed value for the random number generator
 */
void hal_rng_init(uint64_t seed) {
    srand(seed);
}

/**
 * @brief Generate a random byte
 * 
 * @param byte Pointer to store the generated random byte
 * @return uint8_t 1 if successful, 0 if failed
 */
uint8_t hal_rng_get_byte(uint8_t* byte) {
    *byte = (uint8_t)(rand() & 0xFF);
    return 1;
}

/**
 * @brief Stop the random number generator
 * 
 * Empty implementation as stdlib's rand() doesn't need cleanup
 */
void hal_rng_stop(void) {}

/** @} */ // End of HAL_LPC_RNG group
