/**
 * @file drv_rng.c
 * @ingroup DRV_RNG
 * @brief Random Number Generator Driver Implementation
 * @details Implementation of random number generation with uniform distribution
 *          and retry mechanisms.
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include "drv_rng.h"
#include "hal_rng.h"

// Maximum number of retries when getting a random byte
#define RNG_MAX_RETRIES 3

/**
 * @brief Initialize the Random Number Generator
 * 
 * @param seed Initial seed value (may be ignored if hardware RNG is used)
 */
void drv_rng_init(uint64_t seed) {
    hal_rng_init(seed);
}

/**
 * @brief Get a random byte with retries
 * 
 * @param[out] byte Pointer to store the random byte
 * @return uint8_t 1 if successful, 0 if failed after max retries
 */
uint8_t drv_rng_get_byte(uint8_t* byte) {
    if (!byte) {
        return 0;
    }

    uint8_t retries = 0;
    while (retries < RNG_MAX_RETRIES) {
        if (hal_rng_get_byte(byte)) {
            return 1;
        }
        retries++;
    }
    return 0;
}

/**
 * @brief Get a random number in range with uniform distribution
 * 
 * Uses rejection sampling to ensure uniform distribution even for
 * large ranges that are not powers of 2.
 * 
 * @param[in] min Minimum value of the range
 * @param[in] max Maximum value of the range
 * @param[out] result Pointer to store the random number
 * @return uint8_t 1 if successful, 0 if failed
 */
uint8_t drv_rng_get_range(uint32_t min, uint32_t max, uint32_t* result) {
    if (!result || min > max) {
        return 0;
    }

    uint32_t range = max - min + 1;
		// Para asegurar la distribucion uniforme para valores grandes de max
    uint32_t value;
    
        // Get 4 random bytes
		uint32_t random = 0;
		for (int i = 0; i < 4; i++) {
				uint8_t byte;
				if (!drv_rng_get_byte(&byte)) {
						return 0;
				}
				random = (random << 8) | byte;
		}
		value = random % range;
    
    *result = value + min;
    return 1;
}

/**
 * @brief Fill a buffer with random bytes
 * 
 * @param[out] buffer Pointer to the buffer to fill
 * @param[in] length Number of bytes to generate
 * @return uint8_t 1 if successful, 0 if any byte generation fails
 */
uint8_t drv_rng_get_bytes(uint8_t* buffer, uint32_t length) {
    if (!buffer) {
        return 0;
    }

    for (uint32_t i = 0; i < length; i++) {
        if (!drv_rng_get_byte(&buffer[i])) {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Stop the Random Number Generator
 */
void drv_rng_stop(void) {
    hal_rng_stop();
}
