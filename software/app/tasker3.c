/*
 * example of coroutines using setjmp()/longjmp()
 */

#include <hf-risc.h>

#define N_TASKS		4

typedef uint32_t jmp_buf[20];

int32_t _interrupt_set(int32_t s);
int32_t setjmp(jmp_buf env);
void longjmp(jmp_buf env, int32_t val);

static jmp_buf jmp[N_TASKS];
static int cur;

void schedule()
{
	if (!setjmp(jmp[cur])) {
		if (N_TASKS == ++cur)
			cur = 0;
		_interrupt_set(1);
		longjmp(jmp[cur], 1);
	}
}

void timer1ctc_handler(void)
{
	schedule();
}

void idle_task()
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */
        cur = N_TASKS - 1;		/* the first thread to context switch is this one */
	
	setjmp(jmp[N_TASKS - 1]);

	/* enable interrupt mask for TIMER1 CTC events */
	TIMERMASK |= MASK_TIMER1CTC;

	while (1) {			/* thread body */
		printf("[idle task]");
	}
}

void task2(void)
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */
	volatile int cnt = 300000;

	if (!setjmp(jmp[2]))
		idle_task();

	while (1) {			/* thread body */
		printf("[task 2 %d]", cnt++);
	}
}

void task1(void)
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */
	volatile int cnt = 200000;

	if (!setjmp(jmp[1]))
		task2();

	while (1) {			/* thread body */
		printf("[task 1 %d]", cnt++);
	}
}

void task0(void)
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */
	volatile int cnt = 100000;

	if (!setjmp(jmp[0]))
		task1();

	while (1) {			/* thread body */
		printf("[task 0 %d]", cnt++);
	}
}

int main(void)
{
	TIMER1PRE = TIMERPRE_DIV16;

	/* unlock TIMER1 for reset */
	TIMER1 = TIMERSET;
	TIMER1 = 0;

	/* TIMER1 frequency: (39063 * 16) = 250000 cycles (10ms timer @ 25MHz) */
	TIMER1CTC = 15625;

	task0();
    
	return 0;
}
