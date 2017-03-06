#include <hf-risc.h>

volatile int32_t ccount=0, ccount2=0, cmpcount=0, cmp2count=0;

/*
ISRs - interrupt service routines
*/
void counter_handler(void){						// 10.48ms @ 25MHz
	uint32_t m;

	ccount++;
	m = IRQ_MASK;								// read interrupt mask
	m ^= (IRQ_COUNTER | IRQ_COUNTER_NOT);				// toggle timer interrupt mask
	IRQ_MASK = m;								// write to irq mask register
}

void counter_handler2(void){						// 2.62ms @ 25MHz
	uint32_t m;

	ccount2++;
	m = IRQ_MASK;								// read interrupt mask
	m ^= (IRQ_COUNTER2 | IRQ_COUNTER2_NOT);				// toggle timer interrupt mask
	IRQ_MASK = m;								// write to irq mask register
}

void compare_handler(void){
	uint32_t val;

	cmpcount++;
	val = COUNTER;
	val += (CPU_SPEED/1000) * 5;		// 5 ms @ 25MHz
	COMPARE = val;						// update compare reg, clear irq
}

void compare2_handler(void){
	uint32_t val;

	cmp2count++;
	val = COUNTER;
	val += (CPU_SPEED/1000) * 1;		// 1 ms @ 25MHz
	COMPARE2 = val;						// update compare2 reg, clear irq
}

int main(void){
	// register ISRs
	interrupt_register(IRQ_COUNTER, counter_handler);
	interrupt_register(IRQ_COUNTER_NOT, counter_handler);
	interrupt_register(IRQ_COUNTER2, counter_handler2);
	interrupt_register(IRQ_COUNTER2_NOT, counter_handler2);
	interrupt_register(IRQ_COMPARE, compare_handler);
	interrupt_register(IRQ_COMPARE2, compare2_handler);

	// initialize compare registers, clear compare irqs
	COMPARE = COUNTER + (CPU_SPEED/1000) * 5;
	COMPARE2 = COUNTER + (CPU_SPEED/1000) * 1;

	// set interrupt mask (unmask peripheral interrupts)
	IRQ_MASK = (IRQ_COUNTER | IRQ_COUNTER2 | IRQ_COMPARE | IRQ_COMPARE2);

	// global interrupts enable
	IRQ_STATUS = 1;

	ccount=0; ccount2=0; cmpcount=0; cmp2count=0;

	for(;;){
		printf("\ninterrupts -> counter18: %d counter16: %d compare: %d compare2: %d", ccount, ccount2, cmpcount, cmp2count);
	}
}

