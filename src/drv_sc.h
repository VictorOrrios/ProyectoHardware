/* *****************************************************************************
 * Hardware Project 2024
 * 
 * drv_sc.h - Critical Section Driver Interface
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
 *   Interface for the critical section driver module. Provides functions to
 *   manage nested critical sections with proper interrupt control.
 * *****************************************************************************/

#ifndef DRV_SC
#define DRV_SC

#include <stdint.h>

/**
 * @brief Initializes the critical section driver
 */
void drv_sc_iniciar(void);

/**
 * @brief Enter a critical section
 * 
 * Disables interrupts only on first entry to support nesting.
 * 
 * @return uint32_t Current nesting level
 */
uint32_t drv_sc_entrar(void);

/**
 * @brief Exits a critical section, enabling interrupts if this is the outermost exit
 */
void drv_sc_salir(void);

#endif // DRV_SC
