/**
 * @file reserva_gpio_lpc2105.h
 * @ingroup HAL_LPC
 * @brief GPIO Pin Reservations for LPC2105
 * @details Pin definitions and GPIO reservations for the simulated LPC2105 board
 *          in Keil. This file defines the mapping between physical pins and their
 *          functions (LEDs, buttons, monitoring signals).
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */
 
#ifndef RESERVA_GPIO
#define RESERVA_GPIO

/**
 * @brief LED GPIO pin assignments
 * @details Maps each LED to its corresponding GPIO pin number
 */
enum { 	
    LED1_GPIO = 0,    /**< LED 1 connected to GPIO pin 0 */
    LED2_GPIO = 1,    /**< LED 2 connected to GPIO pin 1 */
    LED3_GPIO = 2,    /**< LED 3 connected to GPIO pin 2 */
    LED4_GPIO = 3,    /**< LED 4 connected to GPIO pin 3 */
};

/**
 * @brief Button pins (external interrupts)
 * @details Maps each button to its corresponding external interrupt pin
 */
enum {  
    INT_EXT1 = 14,    /**< Button 1 connected to EINT1 (pin 14) */
    INT_EXT2 = 15,    /**< Button 2 connected to EINT2 (pin 15) */
    INT_EXT3 = 16,    /**< Button 3 connected to EINT0 (pin 16) */
};

/**
 * @brief Monitor signal GPIO pin assignments
 * @details Maps each monitoring signal to its corresponding GPIO pin
 */
enum { 	
    MONITOR1_GPIO = 28,    /**< Monitor signal 1 on GPIO pin 28 */
    MONITOR2_GPIO = 29,    /**< Monitor signal 2 on GPIO pin 29 */
    MONITOR3_GPIO = 30,    /**< Monitor signal 3 on GPIO pin 30 */
    MONITOR4_GPIO = 31,    /**< Monitor signal 4 on GPIO pin 31 */
};

#endif
