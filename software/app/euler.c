/*
approximation of euler's number
algorithm is precise to 9 digits.
*/

#include <hf-risc.h>

int fac(int n){
	int i, f;

	for (i = 1, f = 1; i <= n; i++)
		f = f * i;

	return f;
}

int main(){
	int i;
	double e;
	int8_t buf[30];

	e = 0;
	for (i = 0; i < 13; i++){
		e += 1.0 / (double)fac(i);

		ftoa(e, buf, 6);
		printf("[%d] - e = %s\n", i, buf);
	}
	
	return 0;
}
