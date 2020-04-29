#include <hf-risc.h>

void timer1ctc_handler(void)
{
	printf("TIMER1CTC %d\n", TIMER0);
}

void timer1ocr_handler(void)
{
	static uint32_t tmr1ocr = 0;
	
	if (++tmr1ocr & 1) {
		printf("TIMER1OCR %d\n", TIMER0);
	}
	
	/* schedule next interrupt (half period) */
	TIMER1OCR = (TIMER1OCR + 196) % 19531;
}

int main(void){
	TIMER1PRE = TIMERPRE_DIV256;

	/* TIMER1 base frequency: 5M cycles */
	/* 5000000 / 256 = 19531.25 */
	TIMER1CTC = 19531;
	
	/* TIMER1 alt frequency: 100k cycles */
	/* 100000 / 256 = 390.625 */
	TIMER1OCR = 391;

	/* unlock TIMER1 for reset */
	TIMER1 = TIMERSET;
	TIMER1 = 0;

	/* enable interrupt mask for TIMER1 CTC and OCR events */
	TIMERMASK |= MASK_TIMER1CTC | MASK_TIMER1OCR;

	for(;;){
	}
}
