#include <hf-risc.h>

/*
 * setup TIMER 1, PORTA pin 0 as a oscillator output
 * 
 * TIMER1CTC (timer1, clear on timer compare) is set to 1500.
 * 
 * for F_CLK = 25MHz, OSC frequency is:
 * OSC_CLK = F_CLK (no prescaler)
 * F_OSC = OSC_CLK / 2 / TIMER1CTC = 8333.33 Hz
 */

int main(void)
{
	/* unlock TIMER1 for reset */
	TIMER1 = TIMERSET;
	TIMER1 = 0;
	
	/* TIMER1 OSC frequency */
	TIMER1CTC = 1500;

	/* configure alternate function for PORTA pin 0 output (TIMER1 output, oscillator mode) */
	PAALTCFG0 |= MASK_OSC1;
	
	/* set PORTA pin 0 as an output */
	PADDR |= MASK_P0;
	
	printf("osc set\n");
	
	for (;;);
}
