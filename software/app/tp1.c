/*
 * Earliest Deadline First
 */
#include <hf-risc.h>
#include <list.h>
#include <stdbool.h>

#define N_TASKS 4  
#define READY   0
#define RUNNING 1
#define BLOCKED 2

char mem_pool[8192];

typedef uint32_t jmp_buf[20];
int32_t setjmp(jmp_buf env);
void longjmp(jmp_buf env, int32_t val);

/***** Global Variables *****/
static jmp_buf jmp[N_TASKS];
static int cur;

struct list *taskList; // List contains all the tasks 
struct list *readyList; // List contains only available tasks at the current time

int interr = 0;
int time = 0;
int totalTime;

/***** Struct definition****/
struct task {
    int id;
    int compute;
    int period;
    int deadline;
    int computeCount; // value will be between 0 and compute-1.
	int n_computeCount; // if zero, no over-deadline computation needed, if -2 (e.g) task is over-deadline and owes 2 computations
    int status;

};

void printList (struct list *list){
	int i;
	struct task *printTask;
    printf("id |  c  |  p  |  d  | count | nCount | status\n");
	for (i = 0 ; i< list_count(list) ; i++){
		printTask = list_get(list,i);
		printf(" %d |  %d  |  %d  |  %d  | %d | %d | %d\n",printTask->id, printTask->compute,printTask->period,printTask->deadline,printTask->computeCount,printTask->n_computeCount,printTask->status);
	}
    printf("\n");
}

void updateElement(struct task *newValue, struct list *taskList){
    int i;
    struct task *auxT;
    for(i=0; i< N_TASKS -1; i++){
        auxT = list_get(taskList, i);
        if(auxT->id == newValue->id){
            list_set(taskList, newValue, i);
        }
    }
}


void context_switch(int taskId)
{
    printf("context switch, %d\n", taskId);
	if (!setjmp(jmp[cur])) {
        cur = taskId;
        longjmp(jmp[taskId], 1);
	}
}
// if (id != lastId and taskEnded = false) 
// 	if TRUE, the previous cycle task was blocked. wait timer to continue with interruption
// 	while(!timer_flag)
// 		do something
// 	longjmp(jmp[id], 1); 
// return; // only will reach here when no interruption is needed


void bubble_sort_ready(void)
{
    int k, j;
    for (k = 1; k < list_count(readyList); k++)
    {
		
        for (j = 0; j < list_count(readyList) - 1; j++)
        {
			struct task *task1 = list_get(readyList,j);
			struct task *task2 = list_get(readyList,j+1);

			// Who's deadline is nearest
            if (task1->deadline - time  >  task2->deadline - time) 
            {
                list_set(readyList,task2,j);
				list_set(readyList,task1,j+1);

            }
			// Who's ID's lower
            if ((task1->deadline - time  ==  task2->deadline - time) && (task1->id  >  task2->id)) 
            {
                list_set(readyList,task2,j);
				list_set(readyList,task1,j+1);
            }
			// task2 is over deadline?
            if ((task1->n_computeCount  >  task2->n_computeCount) && task2->n_computeCount < 0) // this condition needs to be reviewed
            {
                list_set(readyList,task2,j);
				list_set(readyList,task1,j+1);
            }
			// If both tasks are over-deadline, Who has the lower ID?
            if ((    task1->n_computeCount < 0 && task2->n_computeCount < 0 ) && (task1->id  >  task2->id) ) 
            {
                list_set(readyList,task2,j);
				list_set(readyList,task1,j+1);
            }
        }
    }
}


void addNewTask(){ // Test if new tasks are ready to be included in readyList
	bool task_added = false;
	for (int i = 0 ; i<N_TASKS-1  ; i++ )  
	{
		struct task *newTask =  list_get(taskList,i);

        if(time == 0){
            list_append(readyList, newTask);
        }
		if (time >= newTask->period) // Test to protect system from value/0
		{   
			if ((time % newTask->period) == 0) // if TRUE, newTask needs to be added to readyList
			{
				for(int j=0; j<list_count(taskList); j++) 
				{
					struct task *auxTask = list_get(readyList,j);
					if (auxTask->id == newTask->id) // Test if newTask is already in readyList, if TRUE, task is over-deadline
					{
						//if (debugmode == 1)
						//	printf("New Task: %c(%d,%d,%d) Already in list \n",tasks[i]->id+64, tasks[i]->c,tasks[i]->p,tasks[i]->d);
						
						auxTask->deadline += time;
						auxTask->n_computeCount = (auxTask->compute - auxTask->computeCount) * -1; //tratar caso n compute time seja diferente de 0
						auxTask->computeCount = 0;
						updateElement(auxTask,readyList);
						task_added = true;
					}
				}
				if ( task_added == false ) // newTask isn't on readyList, so just add to it.
				{
					//if (debugmode == 1)
					//	printf("New Task: %c(%d,%d,%d)\n",tasks[i]->id+64, tasks[i]->c,tasks[i]->p,tasks[i]->d);

					newTask->computeCount = 0;
					newTask->deadline += time;
					list_append(readyList,newTask);
				}

			}
		}
    }
}


void advanceTime() {
	volatile int lastTaskId;
    volatile bool lastTaskEnded = false; 
	bool taskEnded = false;
	// Check if new task are ready
	addNewTask();
    // printf("+++++++++++++++++++++++++++++++++++++");
    // printList(taskList);
    // printf("+++++++++++++++++++++++++++++++++++++");
    // printList(readyList);

	//Sort readyList
	bubble_sort_ready();

	// "run" the task 
	time++;
	struct task *runningTask = list_get(readyList,0);
    printf("%d\n", runningTask->id);
	if (runningTask->n_computeCount != 0 ){
		runningTask->n_computeCount++;
		
		if(runningTask->n_computeCount == 0){  // this code needs to be revised -> aA is an interruption?
			taskEnded = true; 
		}
		updateElement(runningTask, readyList);
	} else  {
		runningTask->computeCount++;
		if (runningTask->computeCount == runningTask->compute){ // END OF TASK
			taskEnded = true;
			list_remove(readyList,0);
		}
	}
    // preempção
    if (runningTask->id != lastTaskId && lastTaskEnded == false) {
        lastTaskId = runningTask->id;
        lastTaskEnded = taskEnded;
        context_switch(runningTask->id);
    } else if(runningTask->id != lastTaskId && lastTaskEnded == true){
        lastTaskId = runningTask->id;
        lastTaskEnded = taskEnded;  
        context_switch(runningTask->id);
    }
	// Actually run the task
}

void idle_task()
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */
        cur = N_TASKS - 1;		/* the first thread to context switch is this one */
		
	setjmp(jmp[N_TASKS - 1]);
    advanceTime(); //  This is needed to decide who will be the first ever task to run. this line should be only run once

	while (1) {			/* thread body */
		
		printf("idle task...\n");
		delay_ms(100);
		advanceTime();
	}
}

void task2_func()
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */

	if (!setjmp(jmp[2]))
		idle_task();

	while (1) {			/* thread body */
		
		printf("task 2...\n");
		delay_ms(100);
		advanceTime();
	}
}

void task1_func()
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */

	if (!setjmp(jmp[1]))
		task2_func();

	while (1) {			/* thread body */
		
		printf("task 2...\n");
		delay_ms(100);
		advanceTime();
	}
}

void task0_func()
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */

	if (!setjmp(jmp[0]))
		task1_func();

	while (1) {			/* thread body */
		
		printf("task 2...\n");
		delay_ms(100);
		advanceTime();
	}
}

int mdc(int a, int b){
    while(b != 0){
        int r = a % b;
        a = b;
        b = r;
    }
    return a;
}

int mmc(int n, int numberList[]){
    int mmcResult = numberList[0];
    int i;
    for(i = 1; i < n; i++){
        mmcResult = mmcResult * (numberList[i] / mdc(mmcResult, numberList[i]));
    }
    return mmcResult;
}


int main(void)
{   
    int32_t i =0;

    /* intialize heap because of list use*/
    heap_init((uint32_t *)&mem_pool, sizeof(mem_pool));

    taskList = list_init();
    readyList = list_init();


    struct task task0, task1, task2;
    task0.id = 0;
    task0.compute  = 1;
    task0.period = 8;
    task0.deadline = 4;
    task0.computeCount = 0;
    task0.status = READY;

    task1.id = 1;
    task1.compute  = 2;
    task1.period = 10;
    task1.deadline = 5;
    task1.computeCount = 0;
    task1.status = READY;

    task2.id = 2;
    task2.compute  = 2;
    task2.period = 15;
    task2.deadline = 10;
    task2.computeCount = 0;
    task2.status = READY;
    
    
    list_append(taskList, &task0);
    list_append(taskList, &task1);
    list_append(taskList, &task2);

    printList(taskList);
    
    /*Calculate total execution time*/
    int vector[N_TASKS-1] = {task0.period, task1.period, task2.period};
    totalTime = mmc(3,vector);
    task0_func();

	return 0;
}