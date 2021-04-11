/*
 * example of coroutines using setjmp()/longjmp()
 */

#include <hf-risc.h>

#define N_TASKS		4

typedef volatile uint32_t jmp_buf[20];

int32_t _interrupt_set(int32_t s);
int32_t setjmp(jmp_buf env);
void longjmp(jmp_buf env, int32_t val);

jmp_buf jmp[N_TASKS];
void (*tasks[N_TASKS])(void) = {[0 ... N_TASKS - 1] = 0};
volatile int cur = 0;
volatile unsigned int ctx_switches = 0;


void idle_task()
{
	volatile char stack[256];	/* reserve some stack space */

	task_init(stack, sizeof(stack), 1);

	while (1) {			/* thread body */
		printf("[idle task]\n");
		task_wfi();
	}
}

void task2(void)
{
	volatile char stack[512];	/* reserve some stack space */
	int cnt = 300000;

	task_init(stack, sizeof(stack), 0);

	while (1) {			/* thread body */
		printf("[task 2 %d]\n", cnt++);
		task_wfi();
	}
}

void task1(void)
{
	volatile char stack[512];	/* reserve some stack space */
	int cnt = 200000;

	task_init(stack, sizeof(stack), 0);

	while (1) {			/* thread body */
		printf("[task 1 %d]\n", cnt++);
		task_wfi();
	}
}

void task0(void)
{
	volatile char stack[512];	/* reserve some stack space */
	int cnt = 100000;

	task_init(stack, sizeof(stack), 0);

	while (1) {			/* thread body */
		printf("[task 0 %d]\n", cnt++);
		task_wfi();
	}
}


void timer1ctc_handler(void)
{
	if (!setjmp(jmp[cur])) {
		if (N_TASKS == ++cur)
			cur = 0;
		ctx_switches++;
		_interrupt_set(1);
		longjmp(jmp[cur], 1);
	}
}

void task_yield()
{
	if (!setjmp(jmp[cur])) {
		if (N_TASKS == ++cur)
			cur = 0;
		ctx_switches++;
		longjmp(jmp[cur], 1);
	}
}

void task_wfi()
{
	volatile unsigned int s;
	
	s = ctx_switches;
	while (s == ctx_switches);
}

int task_add(void *task)
{
	tasks[cur++] = task;
	
	return cur - 1;
}

void task_init(char *stack, int stack_size, char idle)
{
	memset(stack, 0, stack_size);
	
	if (!setjmp(jmp[cur++])) {
		if (idle) {
			cur = N_TASKS - 1;			/* the first thread to context switch is this one */
			TIMERMASK |= MASK_TIMER1CTC;		/* enable interrupt mask for TIMER1 CTC events */
		} else {
			(*tasks[cur])();
		}
	}	
}

void timer_init()
{
	TIMER1PRE = TIMERPRE_DIV16;

	/* unlock TIMER1 for reset */
	TIMER1 = TIMERSET;
	TIMER1 = 0;

	/* TIMER1 frequency: (39063 * 16) = 250000 cycles (10ms timer @ 25MHz) */
	TIMER1CTC = 15625;
}

void sched_init()
{
	cur = 0;
	(*tasks[0])();
}


int main(void)
{
	timer_init();
	task_add(task0);
	task_add(task1);
	task_add(task2);
	task_add(idle_task);
	sched_init();

	return 0;
}
