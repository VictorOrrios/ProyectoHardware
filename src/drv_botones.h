/**
 * @brief Button Driver Interface
 * @file drv_botones.h
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

#endif // DRV_BOTONES
