/* *****************************************************************************
 * P.H.2024: TODO
 * implementacion para cumplir el hal_tiempo.h
 */
/* *****************************************************************************
 * P.H.2024: hal_tiempos, interface que nos independiza del hardware 
 */
 

 
 /* Already included in system_nrf52840.h
#define __SYSTEM_CLOCK_64M (64000000UL)

#if defined ( __CC_ARM )
	uint32_t SystemCoreClock __attribute__((used)) = __SYSTEM_CLOCK_64M;

#endif
*/
 
#ifndef HAL_TIEMPO
#define HAL_TIEMPO

#include <stdint.h>
#include "hal_tiempo.h"
#include "nrf.h"
#include "core_cm4.h"
#include "system_nrf52840.h"

static void(*f_callback)();
static volatile uint64_t systick_counter = 0;


void SysTick_Handler(){
	systick_counter++;
}

// NOTE: 64 bit unsigned to make it as "monotonic" as possible
static volatile uint64_t overflow_counter = 0;

// NOTE: changed from static uint32_t to #define
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
#define MAX_COUNTER_VALUE 0xFFFFFFFF 	// 2^32 (valor máximo del contador de 32 bits)
#define HAL_TICKSPERUS 			16	// 16 MHz clock (16 ticks per 1us)

/**
 * configura e inicializa la cuenta de tiempo en ticks del hardware y
 * devuelve la constante hal_ticks2us,
 * hal_ticks2us permite pasar de los ticks del hardware a microsegundos
 * (tip, el driver lo necesitara para trabajar en us y ms de la app y hacer la conversion a ticks del hardware)
 */
uint32_t hal_tiempo_iniciar_tick(void){
	/*
	// TODO: set to 32Bit (max counter size available)
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
	*/
	
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000000);
	
	return 16;
	
}


void TIMER0_IRQHandler(void){
	volatile uint32_t dummy;
	if (NRF_TIMER0->EVENTS_COMPARE[0] == 1){
		NRF_TIMER0->EVENTS_COMPARE[0]=0;
		dummy = NRF_TIMER0->EVENTS_COMPARE[0];
		dummy;
	}	
	overflow_counter++;
}

void TIMER1_IRQHandler(void){
	volatile uint32_t dummy;
	if (NRF_TIMER1->EVENTS_COMPARE[0] == 1){
		NRF_TIMER1->EVENTS_COMPARE[0]=0;
		// TODO: move outside of the "if" block
		f_callback();
		dummy = NRF_TIMER1->EVENTS_COMPARE[0];
		dummy;
	}	
}



/**
 * nos devuelve el numero total de ticks desde que se inicio la cuenta
 */
uint64_t hal_tiempo_actual_tick(void){
	/*
	// NOTE: must capture in CC[i] for i=1..7, since CC[0] is already taken by MAX_COUNTER_VALUE
	NRF_TIMER0->TASKS_CAPTURE[1] = 1;
	return (uint64_t)(NRF_TIMER0->CC[1] + overflow_counter*(MAX_COUNTER_VALUE));
	*/
	
	uint32_t current = SysTick->VAL;
	return (systick_counter * 1000000) + (SystemCoreClock / 1000000 - current);
	
}


/**
 * TODO para la practica 3, no se si hacer hal propio...
 * programa una activacion periodica cada periodo_en_tick ticks de la maquina
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
		// TODO: check execution order
		NVIC_DisableIRQ(TIMER1_IRQn);
		NRF_TIMER1->TASKS_STOP = 1;
	}
}	

#endif
