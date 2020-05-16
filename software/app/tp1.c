/*
 * Rate Monotonic Scheduler
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
struct list *queue;
int interr = 0;

/***** Struct definition****/
struct task {
    int id;
    int computTime;
    int period;
    int deadline;
    int computTimeCount;
    int status;
};

void context_switch(int taskId)
{
	if (!setjmp(jmp[cur])) {
        cur = taskId;
		longjmp(jmp[cur], 1);
	}
}

struct task *getNextTask(){
    struct task *cTask = list_get(queue, 0);
    int i, auxIndex = 0;
    int menorP = cTask->period;

    for(i=1; i < (N_TASKS-1); i++){
        cTask = list_get(queue, i);
        if(cTask->period < menorP){
            menorP = cTask->period;
            auxIndex = i;
        }
    }
    struct task *returnTask = list_get(queue, auxIndex);
    return returnTask;
}

struct task *getRunningTask(){
    struct task *rTask;
    int i;
    for(i=0; i< (N_TASKS-1); i++){
        rTask = list_get(queue, i);
        if(rTask->status == RUNNING) return rTask;
    }
    return NULL;
}

// Como inicializar o running
void scheduler(){
    struct task *runningT = getRunningTask();
    if(runningT == NULL){
        struct task *curTask = getNextTask();
        context_switch(curTask->id);
    } else if(interr == 1){
        if(runningT->computTimeCount != runningT->computTime){
            //task não acabou de executar ainda e tem que ser preemptada
            //status passa pra BLOCKED
        }
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

int main(void)
{   
    int32_t i =0;

    /* intialize heap because of list use*/
    heap_init((uint32_t *)&mem_pool, sizeof(mem_pool));

    queue = list_init();
    /* reserve space for the incoming tasks */
    while(i < N_TASKS){
        list_append(queue, NULL);
        i++;
    }
    printf("Number of nodes %d\n", list_count(queue));
	
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

    list_append(queue, &task0);
    list_append(queue, &task1);
    list_append(queue, &task2);

    task0_func();

	return 0;
}
