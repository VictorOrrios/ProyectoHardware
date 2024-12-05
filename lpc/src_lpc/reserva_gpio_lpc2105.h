/* *****************************************************************************
 * Hardware Project 2024
 * 
 * reserva_gpio_lpc2105.h - GPIO Pin Reservations for LPC2105
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
 *   Pin definitions and GPIO reservations for the simulated LPC2105 board
 *   in Keil. This file defines the mapping between physical pins and their
 *   functions (LEDs, buttons, monitoring signals).
 * *****************************************************************************/
 
#ifndef RESERVA_GPIO
#define RESERVA_GPIO

// GPIO pin assignments

// LED pins
enum { 	
    LED1_GPIO = 0,
    LED2_GPIO = 1,
    LED3_GPIO = 2,
    LED4_GPIO = 3,
};

// Button pins (external interrupts)
enum {  
    INT_EXT1 = 14,  // eINT1
    INT_EXT2 = 15,  // eINT2
    INT_EXT3 = 16,  // eINT0
};

// Monitor signal pins
enum { 	
    MONITOR1_GPIO = 28,
    MONITOR2_GPIO = 29,
    MONITOR3_GPIO = 30,
    MONITOR4_GPIO = 31,
};

#endif
