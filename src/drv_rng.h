/**
 * @file drv_rng.h
 * @ingroup DRV
 * @brief Random Number Generator Driver Interface
 * @details Interface for the Random Number Generator driver module. Provides
 *          functions to generate random numbers and sequences with uniform
 *          distribution.
 *
 * @defgroup DRV_RNG Random Number Generator Driver
 * @ingroup DRV
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#ifndef DRV_RNG
#define DRV_RNG

#include <stdint.h>

/**
 * @brief Initialize the Random Number Generator
 * 
 * @param seed Initial seed value (may be ignored if hardware RNG is used)
 */
void drv_rng_init(uint64_t seed);

/**
 * @brief Get a random byte
 * 
 * @param[out] byte Pointer to store the random byte
 * @return uint8_t 1 if successful, 0 if failed
 */
uint8_t drv_rng_get_byte(uint8_t* byte);

/**
 * @brief Get a random number between min and max (inclusive)
 * 
 * @param[in] min Minimum value of the range
 * @param[in] max Maximum value of the range
 * @param[out] result Pointer to store the random number
 * @return uint8_t 1 if successful, 0 if failed
 */
uint8_t drv_rng_get_range(uint32_t min, uint32_t max, uint32_t* result);

/**
 * @brief Fill a buffer with random bytes
 * 
 * @param[out] buffer Pointer to the buffer to fill
 * @param[in] length Number of bytes to generate
 * @return uint8_t 1 if successful, 0 if failed
 */
uint8_t drv_rng_get_bytes(uint8_t* buffer, uint32_t length);

/**
 * @brief Stop the Random Number Generator
 */
void drv_rng_stop(void);

/** @} */ // End of DRV_RNG group

#endif // DRV_RNG
