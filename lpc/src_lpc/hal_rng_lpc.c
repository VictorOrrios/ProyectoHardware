/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_rng_lpc.c - Random Number Generator HAL for LPC2105
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
 *   for the LPC2105 microcontroller. This module provides basic random number
 *   generation functionality using the standard C library's rand() function.
 * *****************************************************************************/

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
