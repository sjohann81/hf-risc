/*
 * example of coroutines using setjmp()/longjmp()
 */

#include <hf-risc.h>

#define N_TASKS		4

typedef uint32_t jmp_buf[20];
int32_t setjmp(jmp_buf env);
void longjmp(jmp_buf env, int32_t val);

static jmp_buf jmp[N_TASKS];
static int cur;

void context_switch()
{
	if (!setjmp(jmp[cur])) {
		if (N_TASKS == ++cur)
			cur = 0;
		longjmp(jmp[cur], 1);
	}
}

void idle_task()
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */
        cur = N_TASKS - 1;		/* the first thread to context switch is this one */
	
	setjmp(jmp[N_TASKS - 1]);

	while (1) {			/* thread body */
		context_switch();
		
		printf("idle task...\n");
		delay_ms(100);
	}
}

void task2(void)
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */

	if (!setjmp(jmp[2]))
		idle_task();

	while (1) {			/* thread body */
		context_switch();
		
		printf("task 2...\n");
		delay_ms(100);
	}
}

void task1(void)
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */

	if (!setjmp(jmp[1]))
		task2();

	while (1) {			/* thread body */
		context_switch();
		
		printf("task 1...\n");
		delay_ms(100);
	}
}

void task0(void)
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */

	if (!setjmp(jmp[0]))
		task1();

	while (1) {			/* thread body */
		context_switch();
		
		printf("task 0...\n");
		delay_ms(100);
	}
}

int main(void)
{
	task0();
    
	return 0;
}
