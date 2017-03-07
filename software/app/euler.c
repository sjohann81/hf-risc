/*
approximation of euler's number
algorithm is precise to 9 digits.
*/

#include <hf-risc.h>
#include <fp-math.h>

int fac(int n){
	int i, f;

	for (i = 1, f = 1; i <= n; i++)
		f = f * i;

	return f;
}

int main(){
	int i;
	float e, f;

	e = 0;
	for (i = 0; i < 13; i++){
		e += 1.0 / (float)fac(i);
		f = e;
		printf("[%d] - e = %.9f\n", i, f);
	}

	return 0;
}

