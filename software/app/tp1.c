/*
 * Rate Monotonic Scheduler
 */

#include <hf-risc.h>
#include <list.h>

#define N_TASKS 2 

char mem_pool[8192];

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

//task sample
void task(void)
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

int main(void)
{   
    int32_t i =0;
    struct list *queue;

    /* intialize heap because of list use*/
    heap_init((uint32_t *)&mem_pool, sizeof(mem_pool));


    queue = list_init();
    /* reserve space for the incoming tasks */
    while(i < N_TASKS){
        list_append(queue, NULL);
        i++;
    }
    printf("Number of nodes %d\n", list_count(queue));
	
    //task();
    
	return 0;
}
