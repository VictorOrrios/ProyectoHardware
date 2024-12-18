/* *****************************************************************************
 * Hardware Project 2024
 * 
 * board.h - Board Configuration Header
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
 *   Board configuration header that selects the appropriate pin definitions
 *   and configurations based on the target development board. Part of the
 *   Hardware Abstraction Layer (HAL).
 * *****************************************************************************/

#ifndef BOARD
#define BOARD

#if defined (LPC2105_simulador)
	#include "board_lpc.h"
#elif defined(BOARD_PCA10056)
	#include "board_nrf52840dk.h"
#elif defined(BOARD_PCA10059)
  #include "board_nrf52840_dongle.h"	
#else
	#error "Board is not defined"
#endif
#endif
