/**
 * @file drv_sc.h
 * @ingroup DRV
 * @brief Critical Section Driver Interface
 * @details Interface for the critical section driver module. Provides functions
 *          to manage nested critical sections with proper interrupt control.
 *
 * @defgroup DRV_SC Critical Section Driver
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

/** @} */ // End of DRV_SC group

#endif // DRV_SC
