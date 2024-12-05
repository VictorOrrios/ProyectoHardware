/* *****************************************************************************
 * Hardware Project 2024
 * 
 * hal_tiempo_nrf.c - Timer HAL for nRF52840
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
 *   Implementation of the Timer Hardware Abstraction Layer for the nRF52840
 *   microcontroller. Provides high-precision timing using hardware timers
 *   and supports both SysTick and Timer0/Timer1 implementations.
 * *****************************************************************************/

#ifndef HAL_TIEMPO
#define HAL_TIEMPO

#include <stdint.h>
#include "hal_tiempo.h"
#include "nrf.h"

static void(*f_callback)();

// NOTE: 64 bit unsigned to make it as "monotonic" as possible
static volatile uint64_t overflow_counter = 0;

static volatile uint64_t systick_counter = 0;

/**
 * @brief SysTick interrupt handler
 */
void SysTick_Handler(){
	systick_counter++;
}

/*
 * @Fact (pretty interesting)
 *
 * For a 24-bit counter, the value 16777216 (2^24) would still work for incrementing
 * the overflow counter properly. The reason for this is that the 24 lowest bits of
 * 16777216 are all 0's, so the compare event CC[0] would fire when the timer counter
 * REACHED 0 for the first time, which is RIGHT WHEN the first overflow happens (the
 * initial timer counter value 0 doesn't seem to be able to fire any events). So, the
 * when that overflow happened the timer counter would reset to 0 (or "clear", as set by
 * the configuration of "NRF_TIMER0->SHORTS") and the overflow counter would be effectively
 * incremented.
 * 
 * However, if we set the CC[0] value to 2^24 + 1 (16777217), it wouldn't work. This is
 * because the 24 lowest bits of 2^24 + 1 are all 0's and a 1 (so, "1" in decimal), so
 * the CAPTURE[0] event would fire EVERY TIME the timer counter reached the value "1".
 * Let's put this into perspective:
 * 
 * 	> 	The timer increments its value by 1 unit (1 tick, ~1/16us)
 * 	> 	The COMPARE[0] event fires (as set in NRF_TIMER0->CC[0]),
 * 		and so the TIMER0_IRQn interruption fires as well (as set in NRF_TIMER0->INTENSET)
 * 		and TIMER0_IRQHandler is called. This causes "overflow_counter" to be incremented
 * 		by 1 unit.
 * 	> 	Since "NRF_TIMER0->SHORTS" is set to "TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos",
 * 		the timer counter is reset back to 0 after that COMPARE[0] event has fired.
 * 
 * This will run in an infinite loop, making "overflow_counter" to be incremented by 1 unit
 * EVERY TICK OF THE CLOCK and thus making "hal_tiempo_actual_tick" return an astonishingly
 * high time value (waay far away from the present, since it returns NRF_TIMER0->CC[1] + overflow_counter*(MAX_COUNTER_VALUE),
 * and the return value doesn't overflow since it is a 64-bit unsigned integer) whenever it's
 * called. So, if we wait for that time value returned by "hal_tiempo_actual_tick", the odds are
 * that we'll keep waiting until the end of the universe, and the LED will not switch back to on/off
 * in a reaaally long time (which is just what happened).
 * 
 * Anyway, even though 2^N still works for a timer counter of size N (bits), we'll keep 2^N - 1
 * as the value for "MAX_COUNTER_VALUE" for being formal. Notice that the timer doesn't care about
 * two's complement; it just compares the least N significant bits of whatever number it's been
 * passed in CC[i]. So, we can view it as an unsigned value counter.
 */
#define MAX_COUNTER_VALUE 0xFFFFFFFF 	// 2^32 (max value of the 32 bit counter)
#define HAL_TICKSPERUS 			16		// 16 MHz clock (16 ticks per 1us)

/* Already included in system_nrf52840.h
	#define __SYSTEM_CLOCK_64M (64000000UL)

	#if defined ( __CC_ARM )
	uint32_t SystemCoreClock __attribute__((used)) = __SYSTEM_CLOCK_64M;

	#endif
*/

// Preprocessor directive to use SysTick timer instead of NRF_TIMER0
// #define SYSTICK_TIMER

/**
 * @brief Initialize tick counter
 * 
 * Configures and starts the tick counter hardware and returns the
 * conversion factor from ticks to microseconds.
 * 
 * @return uint32_t Ticks to microseconds conversion factor
 */
uint32_t hal_tiempo_iniciar_tick(void){

	// Conditional compilation to use Systick timer instead of NRF_TIMER0 (blink_v2)
	#ifndef SYSTICK_TIMER
	
	NRF_TIMER0->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
	NRF_TIMER0->PRESCALER = 0 << TIMER_PRESCALER_PRESCALER_Pos;
	
	NRF_TIMER0->CC[0]=MAX_COUNTER_VALUE;
	NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;

	// NOTE: this short-circuit clears the timer counter (sets it to 0) whenever it
	// reaches MAX_COUNTER_VALUE (since we executed "NRF_TIMER0->CC[0]=MAX_COUNTER_VALUE;").
	// If removed, it would still work because of overflow (but it wouldn't look as professional)
	NRF_TIMER0->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
	
	NVIC_EnableIRQ(TIMER0_IRQn);
	
	NRF_TIMER0->TASKS_START = 1;
	
	return HAL_TICKSPERUS;

	#else

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000000);
	
	return 64;

	#endif
	
	
}

/**
 * @brief Timer0 interrupt handler
 */
void TIMER0_IRQHandler(void){
	volatile uint32_t dummy;
	if (NRF_TIMER0->EVENTS_COMPARE[0] == 1){
		NRF_TIMER0->EVENTS_COMPARE[0]=0;
		dummy = NRF_TIMER0->EVENTS_COMPARE[0];
		dummy;
	}	
	overflow_counter++;
}

/**
 * @brief Timer1 interrupt handler
 */
void TIMER1_IRQHandler(void){
	volatile uint32_t dummy;
	if (NRF_TIMER1->EVENTS_COMPARE[0] == 1){
		NRF_TIMER1->EVENTS_COMPARE[0] = 0;
		dummy = NRF_TIMER1->EVENTS_COMPARE[0];
		(void)dummy;
		
		// Call callback when we have a valid timer event
		if (f_callback) {
			f_callback();
		}
	}
}

/**
 * @brief Get current tick count
 * 
 * @return uint64_t Current tick count since initialization
 */
uint64_t hal_tiempo_actual_tick(void){

	// Conditional compilation to use Systick timer instead of NRF_TIMER0
	#ifndef SYSTICK_TIMER

	// NOTE: must capture in CC[i] for i=1..7, since CC[0] is already taken by MAX_COUNTER_VALUE
	NRF_TIMER0->TASKS_CAPTURE[1] = 1;
	return (uint64_t)(NRF_TIMER0->CC[1] + overflow_counter*(MAX_COUNTER_VALUE));

	#else

	uint32_t current = SysTick->VAL;
	return (systick_counter * 64) + (SystemCoreClock / 1000000 - current);

	#endif
}

/**
 * @brief Configure periodic timer
 * 
 * @param periodo_en_tick Period in ticks
 * @param funcion_callback Function to call on each period
 */
void hal_tiempo_reloj_periodico_tick(uint32_t periodo_en_tick, void(*funcion_callback)()){
  
	f_callback = funcion_callback;

	if (periodo_en_tick != 0) {
			// NOTE: using TIMER1, since the IRQ handler for TIMER0 is already busy handling
			// the increment of "overflow_counter"
			NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
			NRF_TIMER1->PRESCALER = 0 << TIMER_PRESCALER_PRESCALER_Pos;
			
			NRF_TIMER1->CC[0]=periodo_en_tick;
			NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
			NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
			
			NVIC_EnableIRQ(TIMER1_IRQn);
			NRF_TIMER1->TASKS_START = 1;
	} else {
		// If "periodo_en_tick" is 0, stop TIMER1 and disable the IRQs for it
		NVIC_DisableIRQ(TIMER1_IRQn);
		NRF_TIMER1->TASKS_STOP = 1;
	}
}

#endif
