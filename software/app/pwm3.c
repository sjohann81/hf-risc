#include <hf-risc.h>

/*
 * setup TIMER 1, PORTA pin 0 as a PWM output
 * 
 * TIMER1CTC (timer1, clear on timer compare) is set to 1000.
 * TIMER1OCR (timer1, output compare ch0) is set to 750 (output low after 75%)
 * 
 * for F_CLK = 25MHz, PWM frequency is:
 * PWM_CLK = F_CLK / 16 = 1562500 Hz
 * F_PWM = PWM_CLK / TIMER1CTC = 1562 Hz
 */

int main(void)
{
	/* set timer prescaler to 16 */
	TIM1->PRE = TIMERPRE_DIV16;
	
	/* reset timer */
	TIM1->COUNT = 0;

	/* TIMER1 PWM duty cycle, 75% */
	TIM1->OCR = 750;
	TIM1->CTC = 1000;
	
	/* configure alternate function for PORTA output */
	/* (TIMER1 output for channel 0, PWM generation) */
	GPIOALT->ALTA |= MASK_TIM1_CH0;
	
	/* set PORTA pin 0 as an output */
	GPIOA->DDR |= MASK_P0;
	
	printf("pwm set\n");
	
	for (;;);
}
