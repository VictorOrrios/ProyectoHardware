/* *****************************************************************************
 * Hardware Project 2024
 * 
 * practica_5_simon.h - Simon Says Game Interface
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
 *   Interface for the Simon Says game module. Provides functions to run
 *   an interactive memory game where players must repeat LED sequences.
 * *****************************************************************************/

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

#endif // PRACTICA_5_SIMON
