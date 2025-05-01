/**
 * @file drv_botones.h
 * @ingroup DRV
 * @brief Button Driver Interface
 * @details Interface for the button driver module. Provides hardware-independent
 *          functions to initialize and handle button events, including debouncing
 *          and double-press detection.
 *
 * @defgroup DRV_BUTTONS Button Driver
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

#ifndef DRV_BOTONES
#define DRV_BOTONES

#include <stdint.h>

/**
 * @brief Initialize the button driver
 * @param callback Callback function for button events
 * @param ID_evento_pulsar Event ID for single press
 * @param ID_evento_retardo Event ID for periodic checks
 * @param ID_evento_doble Event ID for double press
 * @param timeout_doble_ms Maximum time between presses to consider it a double press (in ms)
 * @return uint32_t Number of buttons initialized
 */
uint32_t drv_botones_iniciar(void (*callback)(),
                            uint32_t ID_evento_pulsar, 
                            uint32_t ID_evento_retardo,
                            uint32_t ID_evento_doble,
                            uint32_t timeout_doble_ms);

/** @} */ // End of DRV_BUTTONS group

#endif // DRV_BOTONES
