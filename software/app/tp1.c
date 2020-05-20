/*
 * Earliest Deadline First
 */

#include <hf-risc.h>
#include <list.h>

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
struct list *taskList;
int interr = 0;
int time = 0;
int totalTime;

/***** Struct definition****/
struct task {
    int id;
    int computTime;
    int period;
    int deadline;
    int computTimeCount;
    int status;
    int runTime = 0;
};

void context_switch(int taskId)
{
	if (!setjmp(jmp[cur])) {
        cur = taskId;
		longjmp(jmp[cur], 1);
	}
}

struct task *getNearestDeadline(struct task *task){
    struct task *resultT;
    int j;
    for(j=1; j < N_TASKS-1; j++){
        //rever condição se dsitancia for igual
        if((cTask->deadline - time) > (list_get(taskList, j+1)->deadline - time)){
            resultT = cTask;
            break;
        } else {
            resultT = list_get(taskList, j+1);
        }
    }
    return resultT;        
}

struct task *getNextTask(){
    struct task *cTask, *resultT;
    int i, auxIndex = 0;

    for(i=0; i < (N_TASKS-1); i++){
        cTask = list_get(taskList, i);
        if((cTask->runTime > 0)){
            if((cTask->deadline - (time - cTask->period * cTask->runTime))  < ((time - cTask->period * cTask->runTime))){
                return cTask;
            } else {
                resultT = getNearestDeadline(cTask);
            }
        } else {
            if((cTask->deadline - time) < time) return cTask;
        }
        resultT = getNearestDeadline(cTask);
    }
}

struct task *getRunningTask(){
    struct task *rTask;
    int i;
    for(i=0; i< (N_TASKS-1); i++){
        rTask = list_get(taskList, i);
        if(rTask->status == RUNNING) return rTask;
    }
    return NULL;
}

void updateElement(struct task *newValue){
    int i;
    struct task *auxT;
    for(i=0; i< N_TASKS -1; i++){
        auxT = list_get(taskList, i);
        if(auxT->id == newValue->id){
            list_set(taskList, newValue, i);
        }
    }
}

void scheduler(){
    struct task *runningT = getRunningTask();
    if(runningT == NULL){ // inicaliza a primeira task
        struct task *nextT = getNextTask();
        nextT->status = RUNNING;
        updateElement(nextT);
        context_switch(nextT->id);
    } else {
        //existe uma task rodando já
        time++;
        runningT->computTimeCount++;
        if(runningT->computTimeCount != runningT->computTime-1){
            //task não acabou de executar
            if(getNextTask()->id != runningT->id){
                //go to wait interruption
            }
        } else {
            //task acabou de executar
            runningT->runTime++;
            updateElement(runningT);
            struct task *nextT = getNextTask();
            if(nextT != NULL){
                nextT->status = RUNNING;
                updateElement(nextT);
                context_switch(nextT->id);
            }
        }
        updateElement(runningT); 
    }
}

void idle_task()
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */
        cur = N_TASKS - 1;		/* the first thread to context switch is this one */
	
	setjmp(jmp[N_TASKS - 1]);

	while (1) {			/* thread body */
		scheduler();
		printf("idle task...\n");
		delay_ms(100);
	}
}

void task2_func()
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */

	if (!setjmp(jmp[2]))
		idle_task();

	while (1) {			/* thread body */
		scheduler();
		printf("task 2...\n");
		delay_ms(100);
	}
}

void task1_func()
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */

	if (!setjmp(jmp[1]))
		task2();

	while (1) {			/* thread body */
		scheduler();
		printf("task 2...\n");
		delay_ms(100);
	}
}

void task0_func()
{
	volatile char cushion[1000];	/* reserve some stack space */
	cushion[0] = '@';		/* don't optimize my cushion away */

	if (!setjmp(jmp[0]))
		task1();

	while (1) {			/* thread body */
		scheduler();
		printf("task 2...\n");
		delay_ms(100);
	}
}

int mmc(int n, int numberList[]){
    int mmcResul = numberList[0];
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
    /* reserve space for the incoming tasks */
    while(i < N_TASKS){
        list_append(taskList, NULL);
        i++;
    }
    printf("Number of nodes %d\n", list_count(taskList));
	
    struct task task0, task1, task2;
    task0.id = 0;
    task0.computTime  = 1;
    task0.period = 8;
    task0.deadline = 4;
    task0.computTimeCount = 0;
    task0.status = READY;

    task1.id = 1;
    task1.computTime  = 2;
    task1.period = 10;
    task1.deadline = 5;
    task1.computTimeCount = 0;
    task1.status = READY;

    task2.id = 2;
    task2.computTime  = 2;
    task2.period = 15;
    task2.deadline = 10;
    task2.computTimeCount = 0;
    task2.status = READY;

    list_append(taskList, &task0);
    list_append(taskList, &task1);
    list_append(taskList, &task2);

    /*Calculate total execution time*/
    totalTime = mmc(3, {task0.period, task1.period, task2.period});
    task0_func();

	return 0;
}
