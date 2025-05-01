/**
 * @file drv_consumo.h
 * @ingroup DRV
 * @brief Power Management Driver Interface
 * @details Interface for the power management driver module. Defines functions to
 *          manage different power consumption modes of the microcontroller to
 *          optimize system power usage.
 *
 * @defgroup DRV_POWER Power Management Driver
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

#ifndef DRV_CONSUMO
#define DRV_CONSUMO

#include <stdint.h>

/**
 * @brief Initialize the power management driver
 * 
 * @param mon_wait Monitor ID for wait mode
 * @param mon_dormir Monitor ID for sleep mode
 * 
 * @pre hal_consumo and drv_monitor modules must be available
 */
void drv_consumo_iniciar(uint32_t mon_wait, uint32_t mon_dormir);

/**
 * @brief Put the system in wait mode
 * 
 * Places the system in a low power state from which it can
 * quickly wake up through interrupts.
 * 
 * @pre Driver must be initialized through drv_consumo_iniciar()
 */
void drv_consumo_esperar(void);

/**
 * @brief Put the system in sleep mode
 * 
 * Places the system in a deep low power state.
 * Wake-up time is longer than in wait mode.
 * 
 * @pre Driver must be initialized through drv_consumo_iniciar()
 */
void drv_consumo_dormir(void);

/** @} */ // End of DRV_POWER group

#endif
