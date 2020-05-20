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

char mem_pool[16384];

typedef uint32_t jmp_buf[20];
int32_t setjmp(jmp_buf env);
void longjmp(jmp_buf env, int32_t val);

/***** Global Variables *****/
static jmp_buf jmp[N_TASKS], end;
static int cur;

struct list *taskList; // List contains all the tasks 
struct list *readyList; // List contains only available tasks at the current time
struct list *taskPrintList;

int interrupt = 0;
int time = 0;
int totalTime;

int lastTaskId = N_TASKS;
bool lastTaskEnded = true;

//TODO printar a lista bonitinha
//TODO setar os status das tasks

/***** Struct definition****/
struct task {
    int id;
    int compute;
    int period;
    int deadline;
    int computeCount; // value will be between 0 and compute-1.
	int n_computeCount; // if zero, no over-deadline computation needed, if -2 (e.g) task is over-deadline and owes 2 computations
    int status;
    int runTime;
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

struct task * getTaskById(int id, struct list *givenList){
    int i;
    struct task * aux;
    for(i=0; i<N_TASKS-1;i++){
        aux = list_get(givenList,i);
        if(aux->id == id){
            return aux;
        }
    }
    return NULL;
}

void updateElement(struct task *newValue, struct list *tList){
    int i;
    struct task *auxT;
    for(i=0; i< N_TASKS -1; i++){
        auxT = list_get(tList, i);
        if(auxT->id == newValue->id){
            list_set(tList, newValue, i);
        }
    }
}

void printTaskList(){
    int i;
    for(i=0; i < list_count(taskPrintList)-1;i++){
        int *aux = list_get(taskPrintList,i);
        printf("%c", aux);
    }
}

void context_switch(int taskId){
    while(interrupt == 0){
        printf("");
    }
	if (!setjmp(jmp[cur])) {
        cur = taskId;
        interrupt = 0;
        longjmp(jmp[taskId], 1);
	}
}


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
            if(task1->n_computeCount == 0 && task2->n_computeCount == 0){
                if (task1->deadline - (time - task1->period * task1->runTime)  >  task2->deadline - (time - task2->period * task2->runTime)) 
                {
                    list_set(readyList,task2,j);
                    list_set(readyList,task1,j+1);

                }
                // Who's ID's lower
                if ((task1->deadline - (time - task1->period * task1->runTime)  ==  task2->deadline - (time - task2->period * task2->runTime)) && (task1->id  >  task2->id)) 
                {
                    list_set(readyList,task2,j);
                    list_set(readyList,task1,j+1);
                }
            }
			// task2 is over deadline?
            if (task1->n_computeCount == 0 && task2->n_computeCount < 0) // this condition needs to be reviewed
            {   
                list_set(readyList,task2,j);
				list_set(readyList,task1,j+1);
            }
			// If both tasks are over-deadline, Who has the lower ID?
            if (( task1->n_computeCount < 0 && task2->n_computeCount < 0 ) && (task1->id  >  task2->id) ) 
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
						printf("\nNew Task: %c(%d,%d) Already in list \n", (newTask->id+65), newTask->compute, newTask->deadline);
						
						auxTask->n_computeCount = (auxTask->compute - auxTask->computeCount) * -1; //tratar caso n compute time seja diferente de 0
						auxTask->computeCount = 0;
                        auxTask->status = READY;
						updateElement(auxTask,readyList);
						task_added = true;
					}
				}
				if ( task_added == false ) // newTask isn't on readyList, so just add to it.
				{
                    printf("\nNew Task: %c(%d,%d)\n", (newTask->id+65), newTask->compute, newTask->deadline);
					newTask->computeCount = 0;
                    newTask->status = READY;
					list_append(readyList,newTask);
				}

			}
		}
    }
}

void advanceTime() { 
	bool taskEnded = false;
	int i;
    struct task *taskModel;
    // Check if new task are ready
	addNewTask();

	//Sort readyList
	bubble_sort_ready();
    for(i=1; i<N_TASKS-1;i++){
        taskModel = list_get(readyList, i);
        taskModel->status = READY;
        list_set(readyList, taskModel, i);
    }

	time++;
    
    if(time == totalTime+1){
        //printTaskList();
        longjmp(end,1);
    }
	struct task *runningTask = list_get(readyList,0);
    if(runningTask != NULL){
        runningTask->status =  RUNNING;
        if (runningTask->n_computeCount != 0){
		    runningTask->n_computeCount++;
            list_append(taskPrintList, (int *)(runningTask->id + 97));
            if(runningTask->n_computeCount == 0){  // this code needs to be revised -> aA is an interruption?
                taskEnded = true; 
            }
		    updateElement(runningTask, readyList);
        } else {
            runningTask->computeCount++;
            list_append(taskPrintList, (int *)(runningTask->id + 65));
            if (runningTask->computeCount == runningTask->compute){ // END OF TASK
                taskEnded = true;
                struct task * myTask = getTaskById(runningTask->id, readyList);
                myTask->runTime++;
                list_remove(readyList,0);
            }
        }
        // preempção
        if (runningTask->id != lastTaskId && lastTaskEnded == false) {
            if(lastTaskId != 3){
                struct task *auxTask = getTaskById(lastTaskId, readyList);
                auxTask->status = BLOCKED;
            }
            lastTaskId = runningTask->id;
            lastTaskEnded = taskEnded;
            context_switch(runningTask->id);
        } else if(runningTask->id != lastTaskId && lastTaskEnded == true){
            lastTaskId = runningTask->id;
            lastTaskEnded = taskEnded;  
            context_switch(runningTask->id);
        } else{
            lastTaskEnded = taskEnded;  
        }
    } else {
        lastTaskId = 3;
        lastTaskEnded = taskEnded;
        list_append(taskPrintList, (int *)46);
        context_switch(3);
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
		
		printf("\nTime: %d --> Idle task\n", time);
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
		
		printf("\nTime: %d --> task C\n", time);
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
		
		printf("\nTime: %d --> task B\n", time);
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
		
		printf("\nTime: %d --> task A\n", time);
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

int mmc(int a, int b){
    return a * (b / mdc(a, b));
}
void timer1ctc_handler(void)
{   
    interrupt = 1;
}

void enableTimer(){
    TIMER1PRE = TIMERPRE_DIV64;

	/* unlock TIMER1 for reset */
	TIMER1 = TIMERSET;
	TIMER1 = 0;

	/* TIMER1 frequency: (50000 * 64) = 3200000 cycles */
	TIMER1CTC = 15625;

	/* enable interrupt mask for TIMER1 CTC events */
	TIMERMASK |= MASK_TIMER1CTC;
}

int main(void)
{   
    int32_t i =0;
    /* intialize heap because of list use*/
    heap_init((uint32_t *)&mem_pool, sizeof(mem_pool));

    taskList = list_init();
    readyList = list_init();
    taskPrintList = list_init();


    struct task task0, task1, task2;
    task0.id = 0;
    task0.compute  = 1;
    task0.period = 4;
    task0.deadline = 4;
    task0.computeCount = 0;
    task0.status = READY;

    task1.id = 1;
    task1.compute  = 2;
    task1.period = 5;
    task1.deadline = 5;
    task1.computeCount = 0;
    task1.status = READY;

    task2.id = 2;
    task2.compute  = 6;
    task2.period = 7;
    task2.deadline = 7;
    task2.computeCount = 0;
    task2.status = READY;
    
    
    list_append(taskList, &task0);
    list_append(taskList, &task1);
    list_append(taskList, &task2);

    printList(taskList);
    
    /*Calculate total execution time*/
    totalTime = mmc(mmc(task0.period, task1.period), task2.period);
    printf("Total time: %d", totalTime);
    enableTimer();
    if (!setjmp(end))
        task0_func();
    printf("Simulation result: ");
    printTaskList();
	return 0;
}