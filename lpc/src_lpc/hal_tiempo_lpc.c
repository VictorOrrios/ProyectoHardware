/**
 * @file hal_tiempo_lpc.c
 * @ingroup HAL_LPC
 * @brief Timer HAL implementation for LPC2105
 * @details Implementation of the Timer Hardware Abstraction Layer for the LPC2105
 *          microcontroller. Uses Timer0 for high-precision tick counting with minimal
 *          interrupts, and Timer1 for periodic activation callbacks.
 *
 * @defgroup HAL_LPC_TIMER Timer Management
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

#include <LPC210x.H>
#include <stdint.h>

#define MAX_COUNTER_VALUE 0xFFFFFFFF  // Maximum value for 32-bit counter
#define HAL_TICKS2US     15           // PCLK at 15 MHz from 60 MHz CPU Clock

/* *****************************************************************************
 * Timer0 tick counter
 */

static volatile uint32_t timer0_int_count = 0;  // 32-bit counter for Timer0 ISR occurrences

/* *****************************************************************************
 * Periodic activation with Timer1
 */

static void(*f_callback)();  // Function pointer for Timer1 ISR callback

/**
 * @brief Timer 0 Interrupt Service Routine
 * 
 * Increments the overflow counter and clears the interrupt flag
 */
void timer0_ISR(void) __irq {
    timer0_int_count++;
    T0IR = 1;              // Clear interrupt flag
    VICVectAddr = 0;       // Acknowledge Interrupt
}

/**
 * @brief Timer 1 Interrupt Service Routine
 * 
 * Calls the registered callback function in IRQ mode
 */
void timer1_ISR(void) __irq {
    f_callback();           // Call function from ISR
    T1IR = 1;              // Clear interrupt flag
    VICVectAddr = 0;       // Acknowledge Interrupt
}

/**
 * @brief Initialize tick counter on Timer0
 * 
 * Programs Timer0 for maximum precision and minimum interrupts
 * 
 * @return uint32_t Conversion factor from ticks to microseconds
 */
uint32_t hal_tiempo_iniciar_tick(void) {
    timer0_int_count = 0;	
    T0MR0 = MAX_COUNTER_VALUE;  // One interrupt per microsecond if TC = 1
    T0MCR = 3;                  // Generate interrupt and reset count at MR0

    // Configure IRQ slot 0 of VIC for Timer 0 Interrupt
    VICVectAddr1 = (unsigned long)timer0_ISR;
    // 0x20 enables vectored IRQs, 4 is Timer0 interrupt number
    VICVectCntl1 = 0x20 | 4;                   
    VICIntEnable |= 0x00000010;  // Enable Timer0 Interrupt
	
		// Enable Timer1 Interrupt
		VICVectAddr2 = (unsigned long)timer1_ISR;
		// 0x20 enables vectored IRQs, 5 is Timer1 interrupt number
		VICVectCntl2 = 0x20 | 5; 

    T0TCR = 2;  // Reset counters
    T0TCR = 1;  // Start counting
	
    return HAL_TICKS2US;  // Return ticks to microseconds conversion factor
}


/**
 * @brief Fast divide by 15 using bit magic
 * 
 * @return uint64_t value/15
 */
uint64_t divide_by_15(uint64_t value) {
		// Precision for 1527 days
		// s = 21
		// M = 2^s/15 = 139810
		// 15.0000143...
    return (value * 139810U) >> 21;
}

/**
 * @brief Fast divide by 15000 using bit magic
 * 
 * @return uint64_t value/15000
 */
uint64_t divide_by_15000(uint64_t value) {
		// Precision for 1165 days
		// s = 38
		// M = 2^s/15000 = 18325193
		// 15000.00065...
    return (value * 18325193U) >> 38;
}

/**
 * @brief Get current tick count
 * 
 * @return uint64_t Current tick count since initialization
 */
uint64_t hal_tiempo_actual_tick(void) { 
    return ((MAX_COUNTER_VALUE+1)*timer0_int_count) + (uint64_t)T0TC; 
}

/**
 * @brief Get current time in microseconds
 * 
 * @return uint64_t Total microseconds that has passed since initialization
 */
uint64_t hal_tiempo_actual_us(void){
		return divide_by_15(((MAX_COUNTER_VALUE+1)*timer0_int_count) + (uint64_t)T0TC);
}

/**
 * @brief Get current time in miliseconds
 * 
 * @return uint64_t Total miliseconds that has passed since initialization
 */
uint64_t hal_tiempo_actual_ms(void){
		return divide_by_15000(((MAX_COUNTER_VALUE+1)*timer0_int_count) + (uint64_t)T0TC);
}


/**
 * @brief Configure periodic timer using Timer1
 * 
 * Programs the clock to call the callback function at specified intervals.
 * If period is zero, stops the timer.
 * 
 * @param periodo_en_tick Period in ticks
 * @param funcion_callback Function to call on each period
 */
void hal_tiempo_reloj_periodico_tick(uint32_t periodo_en_tick, void(*funcion_callback)()){
  
	f_callback = funcion_callback;
	
	if (periodo_en_tick != 0) { // If the delay is greater than 0 we start the timer 
		VICIntEnable = VICIntEnable | 0x00000020;  // Enables the interrupt on the VIC
		
		T1TCR = 3;												// Enable counter and reset on every positive edge
		T1MCR = 3;  											// Configuration that makes the counter throw interrupts and resets the counter when it interrupts
		T1MR0 = periodo_en_tick  - 1; 		// 15 Ticks per microsecond
		T1TCR = 1;  											// Start the count and stop reseting
	} else {
		T1TCR = 0;  											// Stop the timer
    VICIntEnClr = 0x20;               // Disable Timer1 Interrupt		
	}
}

/** @} */ // End of HAL_LPC_TIMER group
