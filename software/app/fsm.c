#include <hf-risc.h>

void *s3(void)
{
	printf("final state...\n");
	
	return 0;
}

void *s2(void)
{
	printf("third state...\n");
	
	return s3;
}

void *s1(void)
{
	printf("second state...\n");
	
	return s2;
}

void *s0(void)
{
	printf("first state...\n");
	
	return s1;
}

int main(void)
{
	void *(*state)(void) = s0;
	
	while (state){
		state = (state)();
	}

	return 0;
}
