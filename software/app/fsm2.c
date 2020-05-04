#include <hf-risc.h>

int state1(void){
	printf("state 1: ");
	
	/* returns the event number. 0 stays in the same state */
	return 3;
}

int state2(void){
	printf("state 2: ");
	
	return 2;
}

int state3(void){
	printf("state 3: ");
	
	return 3;
}

int state4(void){
	printf("state 4: ");
	
	return 3;
}

int state5(void){
	printf("state 5: ");
	
	return 1;
}

void noaction(void){
	printf("!");
}

void action1(void){
	printf(" action 1\n");
}

void action2(void){
	printf(" action 2\n");
}

void action3(void){
	printf(" action 3\n");
}

void action4(void){
	printf(" action 4\n");
}

void action5(void){
	printf(" action 5\n");
}

/*
state transition matrix
	event1	event2	event3
state1	state1	state2	state3
state2	state3	state4	state5
state3	state1	state4	state2
state4	state2	state3	state5
state5	state5	state5	state5

state action matrix
noaction action2 action1 action3
noaction action1 action3 action2
noaction action4 action5 action2
noaction action3 action2 action1
noaction action5 action3 action4
*/

int main(void)
{
	int (*state_transition[])(void) = {
		state1, 3, 1, 2,
		state2, 2, 3, 4,
		state3, 0, 3, 1,
		state4, 1, 2, 4,
		state5, 5, 5, 5
	};
	
	void (*state_action[])(void) = {
		noaction, action2, action1, action3,
		noaction, action1, action3, action2,
		noaction, action4, action5, action2,
		noaction, action3, action2, action1,
		noaction, action5, action3, action4
	};
	
	int event, state = 0;

	while (state != 5){
		/* execute state code and return the event that happened on that state */
		event = (state_transition[state * 4])();
		printf("-->event %d", event);
		
		/* execute action code based on the current state and associated action */
		(state_action[state * 4 + event])();
		
		/* go to the next state, based on the current state and event */
		state = state_transition[state * 4 + event];
	}
	
	return 0;
}
