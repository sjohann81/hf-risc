#include <hf-risc.h>

/*
 * setup TIMER 1, PORTA pin 8 as a PWM output
 * 
 * TIMER1CTC (timer1, clear on timer compare) is set to 1000.
 * TIMER1OCR (timer1, output compare) is set to 750 (output low after 750)
 * 
 * for F_CLK = 25MHz, PWM frequency is:
 * PWM_CLK = F_CLK / 4 = 6250000 Hz
 * F_PWM = PWM_CLK / TIMER1CTC = 625 Hz
 */

int main(void)
{
	/* set timer prescaler to 4 */
	TIM1->PRE = TIMERPRE_DIV4;
	
	/* unlock TIMER1 for reset */
	TIM1->COUNT = TIMERSET;
	TIM1->COUNT = 0;

	/* TIMER1 PWM duty cycle, 33% */
	TIM1->OCR = 3333;
	TIM1->CTC = 10000;
	
	/* configure alternate function for PORTA pin 8 output (TIMER1 output, PWM generation) */
	GPIOALT->ALTA |= MASK_PWM1H;
	
	/* set PORTA pin 8 as an output */
	GPIOA->DDR |= MASK_P8;
	
	printf("pwm set\n");
	
	for (;;);
}
