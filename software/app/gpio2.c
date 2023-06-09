#include <hf-risc.h>

void porta_handler(void)
{
	printf("PAIN: %04x\n", PAIN);
}

void portb_handler(void)
{
	printf("PBIN: %04x\n", PBIN);
}

int main(void){
	/* enable interrupt mask for GPIOA and GPIOB inputs */
	GPIO->MASK |= MASK_PAIN | MASK_PBIN;

	/* enable GPIOA input change mask for pins 5 and 4 */
	GPIOA->INMASK |= MASK_P5 | MASK_P4;

	/* enable PORTB input change mask for pins 11, 10 and 1 */
	GPIOB->INMASK |= MASK_P11 | MASK_P10 | MASK_P1;
	
	/* set PORTA pin 0 as an output */
	GPIOA->DDR |= MASK_P0;

	for(;;){
		/* toggle pin 0 */
		GPIOA->OUT ^= MASK_P0;
		
		printf(".");
		delay_ms(1000);
	}
}
