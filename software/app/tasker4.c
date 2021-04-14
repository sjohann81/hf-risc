/*
 * example of coroutines / fibers using setjmp()/longjmp()
 */

#include <hf-risc.h>

#define N_TASKS		10

typedef volatile uint32_t jmp_buf[20];

int32_t _interrupt_set(int32_t s);
int32_t setjmp(jmp_buf env);
void longjmp(jmp_buf env, int32_t val);


/* kernel data / structures */

jmp_buf jmp[N_TASKS];
void (*tasks[N_TASKS])(void) = {[0 ... N_TASKS - 1] = 0};
volatile int cur = 0, n_tasks = 0;
volatile unsigned int ctx_switches = 0;


/* kernel functions */

void schedule(void)
{
	if (!setjmp(jmp[cur])) {
		if (n_tasks == ++cur)
			cur = 0;
		ctx_switches++;
		_interrupt_set(1);
		longjmp(jmp[cur], 1);
	}
}

void timer1ctc_handler(void)
{
	schedule();
}

void task_yield()
{
	if (!setjmp(jmp[cur])) {
		if (n_tasks == ++cur)
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
	n_tasks++;
	
	return cur - 1;
}

void task_init(volatile char *guard, int guard_size)
{
	memset((char *)guard, 0, guard_size);
	
	if (!setjmp(jmp[cur])) {
		if (n_tasks-1 != cur)
			(*tasks[++cur])();
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
	TIMERMASK |= MASK_TIMER1CTC;		/* enable interrupt mask for TIMER1 CTC events */
	(*tasks[0])();
}


/* tasks */

void task2(void)
{
	volatile char guard[1024];		/* reserve some stack space */
	int cnt = 300000;

	task_init(guard, sizeof(guard));

	while (1) {				/* task body */
		printf("[task 2 %d]\n", cnt++);
		task_wfi();			/* wait for an interrupt, to avoid too much text */
	}
}

void task1(void)
{
	volatile char guard[1024];		/* reserve some stack space */
	int cnt = 200000;

	task_init(guard, sizeof(guard));

	while (1) {				/* task body */
		printf("[task 1 %d]\n", cnt++);
		task_wfi();
	}
}

void task0(void)
{
	volatile char guard[1024];		/* reserve some stack space */
	int cnt = 100000;

	task_init(guard, sizeof(guard));

	while (1) {				/* task body */
		printf("[task 0 %d]\n", cnt++);
		task_wfi();
	}
}


/* kernel initialization */

int main(void)
{
	timer_init();
	task_add(task0);
	task_add(task1);
	task_add(task2);
	sched_init();

	return 0;
}
