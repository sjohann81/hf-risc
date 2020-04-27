#include <hf-risc.h>

void timer1ctc_handler(void)
{
	printf("TIMER1CTC %d\n", TIMER0);
}

int main(void){
	TIMER1PRE = TIMERPRE_DIV64;

	/* unlock TIMER1 for reset */
	TIMER1 = TIMERSET;
	TIMER1 = 0;

	/* TIMER1 frequency: (50000 * 64) = 3200000 cycles */
	TIMER1CTC = 50000;

	/* enable interrupt mask for TIMER1 CTC events */
	TIMERMASK |= MASK_TIMER1CTC;

	for(;;){
	}
}
