#include <hf-risc.h>

int fib(int n){
	int a = 0;
	int b = 1;
	int sum;
	int i;

	for (i = n; i > 0; i--) {
		printf("%d ", a);
		sum = a + b;
		a = b;
		b = sum;
	}

	return 0;
}

int main(){
	fib(20);
	putchar('\n');
	
	return 0;
}

