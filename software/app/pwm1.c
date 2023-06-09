#include <hf-risc.h>

/*
 * setup TIMER 1, PORTA pin 0 as a PWM output

 * TIMER1OCR (timer1, output compare) is set to 16383 (output low after 25%)
 * 
 * for F_CLK = 25MHz, PWM frequency is:
 * PWM_CLK = F_CLK (no prescaler)
 * F_PWM = PWM_CLK / 65536 = 381.47 Hz
 */

int main(void)
{
	/* unlock TIMER1 for reset */
	TIMER1 = TIMERSET;
	TIMER1 = 0;

	/* TIMER1 PWM duty cycle, 25% */
	TIMER1OCR = 16383;
	
	/* configure alternate function for PORTA pin 0 output (TIMER1 output, PWM generation) */
	PAALTCFG0 |= MASK_PWM1;
	
	/* set PORTA pin 0 as an output */
	PADDR |= MASK_P0;
	
	printf("pwm set\n");
	
	for (;;);
}
