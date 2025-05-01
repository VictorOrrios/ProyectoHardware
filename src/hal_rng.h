/**
 * @file hal_rng.h
 * @ingroup HAL
 * @brief Random Number Generator HAL Interface
 * @details Interface for the Random Number Generator Hardware Abstraction Layer.
 *          Provides functions to initialize and use hardware RNG capabilities.
 *
 * @defgroup HAL_RNG Random Number Generator HAL
 * @ingroup HAL
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

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
