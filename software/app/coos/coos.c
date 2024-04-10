#include <hf-risc.h>
#include "coos.h"


static struct task_s *ptasks;
static struct task_s *pcurrtask;


/* CoOS API */

/* tasks init, add and schedule (only called on main) */

void task_pinit(struct task_s *tasks)
{
	ptasks = tasks;
	pcurrtask = 0;
}

void task_add(struct task_s *tasks, void *(task_ptr)(void *), unsigned char priority)
{
	struct task_s *ptask;
	struct mq_s *pmqueue;
	int i;
	
	for (i = 0; i < MAX_TASKS; i++) {
		ptask = &tasks[i];
		if (!ptask->task) break;
	}
	
	ptask->task = task_ptr;
	ptask->priority = priority;
	ptask->pcounter = priority;
	
	pmqueue = &ptask->mqueue;
	pmqueue->size = MAX_MESSAGES;
	pmqueue->mask = MAX_MESSAGES - 1;
	pmqueue->head = pmqueue->tail = 0;
	pmqueue->items = 0;
}

void task_schedule(struct task_s *tasks)
{
	struct task_s *ptask;
	int i;
	
	while (1) {
		for (i = 0; i < MAX_TASKS; i++) {
			ptask = &tasks[i];
			if (!ptask->task) break;
			
			if (!--ptask->pcounter) {
				pcurrtask = ptask;
				ptask->pcounter = ptask->priority;
				ptask->task(0);
				return;
			}
		}
	}
}


/* task queue operations (called inside tasks) */

int task_mq_enqueue(void *(task_ptr)(void *), struct message_s *message)
{
	struct task_s *ptask;
	struct mq_s *pmqueue;
	int i;
	int tail;
	
	for (i = 0; i < MAX_TASKS; i++) {
		ptask = &ptasks[i];
		if (!ptask->task) return -1;
		if (ptask->task == task_ptr) break;
	}
	
	pmqueue = &ptask->mqueue;

	tail = (pmqueue->tail + 1) & pmqueue->mask;
	if (tail == pmqueue->head)
		return -1;

	pmqueue->queue[pmqueue->tail] = *message;
	pmqueue->tail = tail;
	pmqueue->items++;
	
	return 0;
}

struct message_s *task_mq_dequeue(void)
{
	struct mq_s *pmqueue;
	int head;

	pmqueue = &pcurrtask->mqueue;

	if (pmqueue->head == pmqueue->tail)
		return 0;

	head = pmqueue->head;
	pmqueue->head = (pmqueue->head + 1) & pmqueue->mask;
	pmqueue->items--;

	return &pmqueue->queue[head];
}

int task_mq_items(void)
{
	struct mq_s *pmqueue;

	pmqueue = &pcurrtask->mqueue;
	
	return pmqueue->items;
}
