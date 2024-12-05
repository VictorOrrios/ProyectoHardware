/* *****************************************************************************
 * Hardware Project 2024
 * 
 * drv_sc.c - Critical Section Driver Implementation
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
 *   Implementation of the critical section driver module. Provides nested
 *   critical section support by tracking the nesting depth and preserving
 *   the interrupt state. Only disables interrupts on first entry and
 *   restores original state on last exit.
 * *****************************************************************************/

#include "drv_sc.h"
#include "hal_sc.h"

// NOTE: not volatile as it's protected by interrupt disable
static uint32_t m_in_critical_region = 0;

// Stores the interrupt state from first critical section entry
static uint32_t m_initial_state = 0;

/**
 * @brief Initialize the critical section driver
 */
void drv_sc_iniciar(void) {
    m_in_critical_region = 0;
    hal_sc_iniciar();
}

/**
 * @brief Enter a critical section
 * 
 * @return uint32_t Current nesting level
 */
uint32_t drv_sc_entrar(void) {
    // Only disable interrupts on first entry
    if (m_in_critical_region == 0) {
        m_initial_state = hal_sc_entrar();
    }
    
    return m_in_critical_region++;
}

/**
 * @brief Exit a critical section
 */
void drv_sc_salir() {
    // Prevent underflow
    if (m_in_critical_region == 0) {
        return;
    }
    
    m_in_critical_region--;
    
    // Only restore interrupts when exiting outermost critical section
    if (m_in_critical_region == 0) {
        hal_sc_salir(m_initial_state);
    }
}
