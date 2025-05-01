/**
 * @file practica_5.h
 * @ingroup APP
 * @brief Simon Says Game Interface
 * @details Interface for the Simon Says game module. Provides functions to run
 *          an interactive memory game where players must repeat LED sequences.
 *
 * @defgroup APP_SIMON Simon Says Game
 * @ingroup APP
 * @details This module implements a complete Simon Says game:
 *          - Multiple difficulty levels
 *          - Sequence generation and validation
 *          - Button input processing
 *          - Visual feedback through LEDs
 *          - Game state management
 *          - Performance statistics
 *          - Watchdog protection
 *          - Power management
 * @{
 */

#ifndef PRACTICA_5_SIMON
#define PRACTICA_5_SIMON

#include <stdint.h>

/**
 * @brief Launch Simon Says game
 * 
 * Initializes and starts the Simon Says game with specified parameters.
 * 
 * @param num_leds Number of LEDs available for the game
 * @param num_botones Number of buttons available for input
 * @param is_reset Flag indicating if this is a game reset (1) or new game (0)
 */
void simon_launcher(uint32_t num_leds, uint32_t num_botones, uint32_t is_reset);

/** @} */ // End of DRV_BUTTONS group

#endif // PRACTICA_5_SIMON
