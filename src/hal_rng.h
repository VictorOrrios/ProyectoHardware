/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_rng.h - Random Number Generator HAL Interface
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
 *   Interface for the Random Number Generator Hardware Abstraction Layer.
 *   Provides functions to initialize and use hardware random number
 *   generation capabilities.
 * *****************************************************************************/

#ifndef HAL_RNG
#define HAL_RNG

#include <stdint.h>

/**
 * @brief Initialize the Random Number Generator hardware
 * 
 * @param seed Initial seed value (may be ignored if true hardware RNG is used)
 */
void hal_rng_init(uint64_t seed);

/**
 * @brief Get a random byte from the hardware
 * 
 * @param[out] byte Pointer to store the random byte
 * @return uint8_t 1 if byte was successfully generated, 0 if failed
 */
uint8_t hal_rng_get_byte(uint8_t* byte);

/**
 * @brief Stop the Random Number Generator hardware
 */
void hal_rng_stop(void);

#endif // HAL_RNG
