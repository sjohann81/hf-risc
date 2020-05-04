/* example of coroutines */

#include <hf-risc.h>

void task1(void)
{
	printf("task 1\n");
}

void task2(void)
{
	printf("task 2\n");
}

void task3(void)
{
	printf("task 3\n");
}

int main(void)
{
	void (*task_sched[])(void) = {task1, task3, task2, task2, task3};
	int i = 0;
	
	while (1) {
		(*task_sched[i++])();		
		if (i == sizeof(task_sched) / sizeof(void *)) i = 0;
	}
}
