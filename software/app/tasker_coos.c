#include <hf-risc.h>
#include "coos/coos.h"

void *t1(void *arg);
void *t2(void *arg);
void *t3(void *arg);
void *t4(void *arg);

void *t1(void *arg)
{
	static int val = 0;
	static struct message_s msg1, msg2;
	static char str[50];
	struct message_s *pmsg;
	
	printf("t1\n");
	
	if (task_mq_items() == 0)
		if (val > 0) return 0;

	printf("task 1 enters...\n");
	
	if (task_mq_items())
		task_mq_dequeue();

	pmsg = &msg1;
	pmsg->data = (void *)(size_t)val;
	task_mq_enqueue(t2, pmsg);
	pmsg = &msg2;
	sprintf(str, "hello %d from t1...", val++);
	pmsg->data = (void *)&str;
	task_mq_enqueue(t3, pmsg);

	return 0;
}

void *t2(void *arg)
{
	static struct message_s msg1;
	static int val = 200;
	struct message_s *msg;
	
	printf("t2\n");
	
	if (task_mq_items() > 0) {
		printf("task 2 enters...\n");

		msg = task_mq_dequeue();
		printf("message %d\n", (int)(size_t)msg->data);
		msg = &msg1;
		msg->data = (void *)(size_t)val++;
		task_mq_enqueue(t4, msg);
	}
	
	return 0;
}

void *t3(void *arg)
{
	static struct message_s msg1;
	static int val = 300;
	struct message_s *msg;
	
	printf("t3\n");
	
	if (task_mq_items() > 0) {
		printf("task 3 enters...\n");
		
		msg = task_mq_dequeue();
		printf("message: %s\n", (char *)msg->data);
		msg = &msg1;
		msg->data = (void *)(size_t)val++;
		task_mq_enqueue(t4, msg);
	}
	
	return 0;
}

void *t4(void *arg)
{
	struct message_s *msg1, *msg2;
	static struct message_s dummy;
	
	printf("t4\n");
	
	if (task_mq_items() > 1) {
		printf("task 4 enters...\n");

		msg1 = task_mq_dequeue();
		msg2 = task_mq_dequeue();
		printf("messages: %d %d\n", (int)(size_t)msg1->data, (int)(size_t)msg2->data);
		
		delay_ms(100);
		
		task_mq_enqueue(t1, &dummy);
	}
	
	return 0;
}

int main(void)
{
	struct task_s tasks[MAX_TASKS] = { 0 };
	struct task_s *ptasks = tasks;

	/* setup CoOS and tasks */
	task_pinit(ptasks);
	task_add(ptasks, t1, 30);
	task_add(ptasks, t2, 50);
	task_add(ptasks, t3, 30);
	task_add(ptasks, t4, 60);

	while (1) {
		task_schedule(ptasks);
	}
	
	/* never reached */
	return 0;
}
