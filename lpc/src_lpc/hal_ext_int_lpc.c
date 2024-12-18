/**
 * @file hal_ext_int_lpc.c
 * @ingroup HAL_LPC
 * @brief External Interrupts HAL implementation for LPC2105
 * @details Implementation of the External Interrupts Hardware Abstraction Layer for
 *          the LPC2105 microcontroller. This module manages external interrupt
 *          configuration and handling, particularly for button inputs.
 *
 * @defgroup HAL_LPC_EXTINT External Interrupts
 * @ingroup HAL_LPC
 * @{
 *
 * Hardware Project 2024
 * EINA - University of Zaragoza
 *
 * @author Víctor Orrios Barón (840994)
 * @author José Miguel Quílez Vergara (873499)
 * @date 17/12/2024
 */

#include <LPC210x.H> /* LPC210x definitions */
#include "hal_ext_int.h"

static void (*ext_int_callback)(uint32_t pin) = 0;

static uint8_t estados [3];

/**
 * @brief Register a callback function for external interrupts
 * 
 * This function allows registering a handler that will be called when an
 * external interrupt occurs. The callback will receive the pin number
 * that generated the interrupt as a parameter.
 * 
 * @param callback Function to call when an external interrupt occurs.
 *                 If NULL, notifications will be disabled.
 */
void hal_ext_int_registrar_callback(void (*callback)(uint32_t)) {
    ext_int_callback = callback;
}

/**
 * @brief Interrupt Service Routine for External Interrupt 0
 * 
 * Handles interrupts from BUTTON_3. Clears the interrupt flag,
 * calls the callback if registered, and disables further interrupts
 * until the button is released.
 */
void eint0_ISR (void) __irq {
	EXTINT |= 0b1;
	VICVectAddr = 0;
	if(!estados[0]){
		ext_int_callback(16);
	}
	estados[0] = 1;
	hal_ext_int_deshabilitar_int(16);
}

/**
 * @brief Interrupt Service Routine for External Interrupt 1
 * 
 * Handles interrupts from BUTTON_1. Clears the interrupt flag,
 * calls the callback if registered, and disables further interrupts
 * until the button is released.
 */
void eint1_ISR (void) __irq {
	EXTINT |= 0b10;
	VICVectAddr = 0;
	if(!estados[1]){
		ext_int_callback(14);
	}
	estados[1] = 1;
	hal_ext_int_deshabilitar_int(14);
}

/**
 * @brief Interrupt Service Routine for External Interrupt 2
 * 
 * Handles interrupts from BUTTON_2. Clears the interrupt flag,
 * calls the callback if registered, and disables further interrupts
 * until the button is released.
 */
void eint2_ISR (void) __irq {
	EXTINT |= 0b100;
	VICVectAddr = 0;
	if(!estados[2]){
		ext_int_callback(15);
	}
	estados[2] = 1;
	hal_ext_int_deshabilitar_int(15);
}

/**
 * @brief Initialize the external interrupt system
 * 
 * Configure the hardware needed to manage external interrupts
 * and the ability to wake up from low power mode.
 */
void hal_ext_int_iniciar(void){
	
	EXTINT |= 0b111;        // clear interrupt flag for all buttons     	
	// configuration of the IRQ slot number 2, 3 & 4 of the VIC for EXTINT0,1 & 2
	VICVectAddr3 = (unsigned long)eint0_ISR;          // set interrupt vector
	VICVectAddr4 = (unsigned long)eint1_ISR;          // set interrupt vector
	VICVectAddr5 = (unsigned long)eint2_ISR;          // set interrupt vector
	
	
	// Pin select block configuration
	PINSEL0 &= 0x0FFFFFFF;	
	PINSEL1 &= 0xFFFFFFFC;	
	PINSEL0	|= 0xA0000000; // //Enable the EXTINT1 & EXTINT2 interrupt in pin select block
	PINSEL1 |= 1;					//Enable the EXTINT0 interrupt in pin select block
	
	
	// Priority setup
	VICVectCntl3 &= 0xFFFFFFC0;
	VICVectCntl4 &= 0xFFFFFFC0;
	VICVectCntl5 &= 0xFFFFFFC0;
	VICVectCntl3 = 0x20 | 14;   
	VICVectCntl4 = 0x20 | 15;   
	VICVectCntl5 = 0x20 | 16;  
	
	estados[0] = 0;
	estados[1] = 0;
	estados[2] = 0;
	
	ext_int_callback = 0;				 // Initialize callback to 0
}

/**
 * @brief Enable external interrupts for a pin
 * 
 * @param pin Pin number for which to enable interrupts
 */
void hal_ext_int_habilitar_int(uint32_t pin){
	switch(pin){
		case 14: 
			//PINSEL0 |= 1 <<29;
			VICIntEnable |= 1 << 15; 
		break;
		case 15: 
			//PINSEL0 |= 1 << 31;
			VICIntEnable |= 1 << 16;
		break;
		case 16: 
			//PINSEL1 |= 1;
			VICIntEnable |= 1 << 14; 
		break;
	}
}

/**
 * @brief Disable external interrupts for a pin
 * 
 * @param pin Pin number for which to disable interrupts
 */
void hal_ext_int_deshabilitar_int(uint32_t pin){
	switch(pin){
		case 14: 
			//PINSEL0 &= 0xDFFFFFFF;
			VICIntEnClr |= 1 << 15;
			EXTINT |= 0b10;
		break;
		case 15: 
			//PINSEL0 &= 0x7FFFFFFF;
			VICIntEnClr |= 1 << 16;
			EXTINT |= 0b100; 
		break;
		case 16: 
			//PINSEL1 &= ~1;
			VICIntEnClr |= 1 << 14;
			EXTINT |= 0b1; 
		break;
	}
}

/**
 * @brief Get the current state of a gpio pin
 * 
 * @param pin Pin number to check
 * @return uint8_t Current state of the pin
 */
uint8_t hal_ext_int_get_estado_pin(uint32_t pin){
	switch(pin){
		case 14:
			EXTINT |= 0b10;
			if(EXTINT & 0b10 && estados[1]){
				return 0;
			}else{
				estados[1] = 0;
				return 1;
			}
		case 15: 
			EXTINT |= 0b100;
			if(EXTINT & 0b100 && estados[2]){
				return 0;
			}else{
				estados[2] = 0;
				return 1;
			}
		case 16: 
			EXTINT |= 0b1;
			if(EXTINT & 0b1 && estados[0]){
				return 0;
			}else{
				estados[0] = 0;
				return 1;
			}
	}
	return 1;
}

/**
 * @brief Enable wake-up capability for a pin
 * 
 * @param pin Pin number that will be able to wake up the system
 */
void hal_ext_int_habilitar_despertar(uint32_t pin){
	switch(pin){
		case 14: 
			EXTWAKE |= 0b010;
		break;
		case 15: 
			EXTWAKE |= 0b100;
		break;
		case 16: 
			EXTWAKE |= 0b001;
		break;
	}
}

/**
 * @brief Disable wake-up capability for a pin
 * 
 * @param pin Pin number that will no longer be able to wake up the system
 */
void hal_ext_int_deshabilitar_despertar(uint32_t pin){
	switch(pin){
		case 14: 
			EXTWAKE &= ~0b010;
		break;
		case 15: 
			EXTWAKE &= ~0b100;
		break;
		case 16: 
			EXTWAKE &= ~0b001;
		break;
	}
}

/** @} */ // End of HAL_LPC_EXTINT group
