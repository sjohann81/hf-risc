#include <hf-risc.h>
#include "coos/coos.h"

/* application tasks */
void *task1(void *arg);
void *task2(void *arg);
void *task3(void *arg);

void *task1(void *arg)
{
	static int cont = 0;
	struct message_s msg;
	struct message_s *pmsg = &msg;
	
	printf("task 1, cont: %d\n", cont++);
	
	if ((cont % 10) == 0) {
		pmsg->type = 0;
		pmsg->data = (void *)123;
		task_mq_enqueue(task3, pmsg);
	}
	
	return 0;
}

void *task2(void *arg)
{
	static int cont = 0;
	
	printf("task 2, cont: %d\n", cont++);
	
	return 0;
}

void *task3(void *arg)
{
	static int cont = 0;
	struct message_s *pmsg;
	
	printf("task 3, cont: %d\n", cont++);
	if (task_mq_items() > 0) {
		pmsg = task_mq_dequeue();
		printf("received (%d): type: %d, data: %ld\n", task_mq_items(), pmsg->type, (long)pmsg->data);
	}
	
	return 0;
}

int main(void)
{
	struct task_s tasks[MAX_TASKS] = { 0 };
	struct task_s *ptasks = tasks;

	/* setup CoOS and tasks */
	task_pinit(ptasks);
	task_add(ptasks, task1, 128);
	task_add(ptasks, task2, 75);
	task_add(ptasks, task3, 50);

	while (1) {
		task_schedule(ptasks);
	}
	
	/* never reached */
	return 0;
}
