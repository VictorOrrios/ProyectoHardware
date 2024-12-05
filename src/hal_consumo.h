/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_consumo.h - Power Management HAL Interface
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
 *   Interface for the Power Management Hardware Abstraction Layer.
 *   Provides functions to manage different power consumption modes
 *   of the microcontroller, allowing power optimization based on
 *   application needs.
 * *****************************************************************************/

#ifndef HAL_CONSUMO
#define HAL_CONSUMO

#include <stdint.h>

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
