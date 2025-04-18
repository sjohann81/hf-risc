#include <hf-risc.h>

/*
 * setup TIMER 1, PORTA pins 0 and 1 as a PWM output

 * TIMER1OCR (timer1, output compare ch0) is set to 32767 (output low after 50%)
 * TIMER1OCR1 (timer1, output compare ch1) is set to 6553 (output low after 10%)
 * 
 * for F_CLK = 25MHz, PWM frequency is:
 * PWM_CLK = F_CLK / 4
 * F_PWM = PWM_CLK / 65536 = 95.36 Hz
 */

int main(void)
{
	/* set timer prescaler to 4 */
	TIMER1PRE = TIMERPRE_DIV4;
	
	/* reset timer */
	TIMER1 = 0;

	/* TIMER1 PWM duty cycle, ch0 @ 50%, ch1 @10% */
	TIMER1OCR = 32767;
	TIMER1OCR1 = 6553;
	
	/* configure alternate function for PORTA */
	/* (TIMER1 output for channels 0 and 1, PWM generation) */
	PAALTCFG0 |= MASK_TIM1_CH0 | MASK_TIM1_CH1;
	
	/* set PORTA pins 0 and 1 as outputs */
	PADDR |= MASK_P0 | MASK_P1;
	
	printf("pwm set\n");
	
	for (;;);
}
