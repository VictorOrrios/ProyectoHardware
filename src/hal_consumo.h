/**
 * @file hal_consumo.h
 * @ingroup HAL
 * @brief Power Management HAL Interface
 * @details Interface for the Power Management Hardware Abstraction Layer.
 *          Provides functions to manage different power consumption modes
 *          of the microcontroller.
 *
 * @defgroup HAL_PM Power Management HAL
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

#ifndef HAL_CONSUMO
#define HAL_CONSUMO

/**
 * @brief Initialize power management system
 * 
 * Configures necessary registers to enable different
 * low power modes of the microcontroller.
 */
void hal_consumo_iniciar(void);

/**
 * @brief Put microcontroller in wait mode
 * 
 * In this mode, the processor stops execution but keeps peripherals
 * active. Wakes up on any interrupt. Moderate power consumption.
 */
void hal_consumo_esperar(void);

/**
 * @brief Put microcontroller in sleep mode
 * 
 * Minimum power consumption mode where most peripherals and main
 * clock are stopped. Only essential peripherals configured for
 * wake-up remain active.
 */
void hal_consumo_dormir(void);

#endif
