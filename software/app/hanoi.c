#include <hf-risc.h>

int moves = 0;

/* disk, source, dest, spare */
int hanoi(int n, int a, int b, int c){
	if (n == 1){
		printf("move disc %d from peg %d to peg %d\n", n, a, b);
	}else{
		hanoi(n-1, a, c, b);
		printf("move disc %d from peg %d to peg %d\n", n, a, b);
		hanoi(n-1, c, b, a);
	}
	moves++;
}

int main(){
	hanoi(10, 1, 3, 2);
	printf("done in %d moves.\n", moves);
}
