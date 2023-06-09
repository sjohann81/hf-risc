#include <hf-risc.h>

/*
 * setup TIMER 1, PORTA pin 0 as a PWM output
 * 
 * TIMER1CTC (timer1, clear on timer compare) is set to 1000.
 * TIMER1OCR (timer1, output compare) is set to 750 (output low after 750)
 * 
 * for F_CLK = 25MHz, PWM frequency is:
 * PWM_CLK = F_CLK / 16 = 1562500 Hz
 * F_PWM = PWM_CLK / TIMER1CTC = 1562 Hz
 */

int main(void)
{
	/* set timer prescaler to 16 */
	TIMER1PRE = TIMERPRE_DIV16;
	
	/* unlock TIMER1 for reset */
	TIMER1 = TIMERSET;
	TIMER1 = 0;

	/* TIMER1 PWM duty cycle, 75% */
	TIMER1OCR = 750;
	TIMER1CTC = 1000;
	
	/* configure alternate function for PORTA pin 0 output (TIMER1 output, PWM generation) */
	PAALTCFG0 |= MASK_PWM1;
	
	/* set PORTA pin 0 as an output */
	PADDR |= MASK_P0;
	
	printf("pwm set\n");
	
	for (;;);
}
