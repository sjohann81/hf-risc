#include <hf-risc.h>

/*
 * setup TIMER 1, PORTA pins 8 and 9 as a PWM output
 * 
 * TIMER1CTC (timer1, clear on timer compare) is set to 1000.
 * TIMER1OCR (timer1, output compare) is set to 3333 (output low after 33%)
 * TIMER1OCR1 (timer1, output compare ch1) is set to an up and down pattern
 * 
 * for F_CLK = 25MHz, PWM frequency is:
 * PWM_CLK = F_CLK / 4 = 6250000 Hz
 * F_PWM = PWM_CLK / TIMER1CTC = 625 Hz
 */

int main(void)
{
	/* set timer prescaler to 4 */
	TIM1->PRE = TIMERPRE_DIV4;
	
	/* reset timer */
	TIM1->COUNT = 0;

	/* TIMER1 PWM duty cycle, ch0 @ 33%, reset ch1 */
	TIM1->OCR = 3333;
	TIM1->CTC = 10000;
	TIM1->OCR1 = 0;
	
	/* configure alternate function for PORTA, high part of port */
	/* (TIMER1 output for channels 0 and 1, PWM generation) */
	GPIOALT->ALTA |= MASK_TIM1H_CH0 | MASK_TIM1H_CH1;
	
	/* set PORTA pin 8 as an output */
	GPIOA->DDR |= MASK_P8 | MASK_P9;
	
	printf("pwm set\n");
	
	while (1) {
		printf("TIM1->OCR1 up\n");
		for (int i = 0; i < TIM1->CTC; i++) {
			TIM1->OCR1 = i;
			delay_us(500);
		}
		printf("TIM1->OCR1 down\n");
		for (int i = TIM1->CTC; i >= 0; i--) {
			TIM1->OCR1 = i;
			delay_us(500);
		}	
	}
}
