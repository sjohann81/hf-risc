/*
approximation of euler's number using fixed point math
*/

#include <hf-risc.h>
#define FIX_IBITS		12
#include <fixed.h>

int fac(int n){
	int i, f;

	for (i = 1, f = 1; i <= n; i++)
		f = f * i;

	return f;
}

int main(){
	int i;
	fixed_t e;
	int8_t buf[30];

	printf("\nFIX_IBITS: %d FIX_FBITS: %d FIX_FMASK: %08x\n", FIX_IBITS, FIX_FBITS, FIX_FMASK);

	e = fix_val(0.0);
	for (i = 0; i < 13; i++){
		e += fix_div(fix_val(1.0), fix_val(fac(i)));
		
		fixtoa(e, buf, 6);
		printf("[%d] - e = %s\n", i, buf);
	}
	
	return 0;
}
