#include <hf-risc.h>

/*
 * setup TIMER 1, PORTA pin 0 as a PWM output

 * TIMER1OCR (timer1, output compare ch0) is set to 16383 (output low after 25%)
 * 
 * for F_CLK = 25MHz, PWM frequency is:
 * PWM_CLK = F_CLK (no prescaler)
 * F_PWM = PWM_CLK / 65536 = 381.47 Hz
 */

int main(void)
{
	/* reset timer */
	TIMER1 = 0;

	/* TIMER1 PWM duty cycle, 25% */
	TIMER1OCR = 16383;
	
	/* configure alternate function for PORTA output */
	/* (TIMER1 output for channel 0, PWM generation) */
	PAALTCFG0 |= MASK_TIM1_CH0;
	
	/* set PORTA pin 0 as an output */
	PADDR |= MASK_P0;
	
	printf("pwm set\n");
	
	for (;;);
}
