/*
approximation of euler's number using fixed point math
*/

#include <hf-risc.h>
#define FIX_IBITS	20
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
	fixtoa(fix_sin(fix_val(30.0)), buf, 6);
	printf("sin 30: %s\n", buf);
	fixtoa(fix_sin(fix_val(45.0)), buf, 6);
	printf("sin 45: %s\n", buf);
	fixtoa(fix_sqrt(fix_val(2.0)), buf, 6);
	printf("sqrt 2: %s\n", buf);
	fixtoa(fix_exp(fix_val(5.0)), buf, 6);
	printf("exp 5: %s\n", buf);
	fixtoa(fix_ln(fix_val(2.0)), buf, 6);
	printf("ln 2: %s\n", buf);
	fixtoa(fix_log(fix_val(2.0), fix_val(10.0)), buf, 6);
	printf("log (2,10): %s\n", buf);
	fixtoa(fix_pow(fix_val(2.0), fix_val(3.0)), buf, 6);
	printf("pow (2,3): %s\n", buf);
	fixtoa(fix_mul(fix_div(fix_val(1.0), fix_val(9.0)), fix_val(9.0)), buf, 6);
	printf("(1.0 / 9.0) * 9.0: %s\n", buf); 
	return 0;
}
