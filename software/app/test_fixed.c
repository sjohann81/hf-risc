#include <hf-risc.h>
#include <fixed.h>

fixed_t epsilon(void){
	fixed_t x = fix_val(1.0);
	
	while ((fix_val(1.0) + fix_div(x, fix_val(2.0))) > fix_val(1.0))
		x = fix_div(x, fix_val(2.0));
		
	return x;
}

void doit(fixed_t a, fixed_t x){
	char buf[30], buf2[30];
	
	fixtoa(a, buf, 6); fixtoa(x, buf2, 6);
	printf("\n\na = %s, x = %s\n", buf , buf2);
	fixtoa(x, buf, 6);
	printf("\nx                                            = %s", buf);
	fixtoa((a + x) - a, buf, 6);
	printf("\n(a + x) - a                                  = %s", buf);
	fixtoa(fix_mul(fix_val(4.0), (a + x)) - fix_mul(fix_val(3.0), x) - fix_val(3.0), buf, 6);
	printf("\n4.0 * (a + x) - 3 * x - 3                    = %s", buf);
	fixtoa(fix_mul(fix_val(0.2), (a + x)) + fix_mul(fix_val(0.8), x) - fix_val(0.15), buf, 6);
	printf("\n0.2 * (a + x) + 0.8 * x - 0.15               = %s", buf);
	fixtoa(fix_div(x, (a + x)), buf, 6);
	printf("\nx / (a + x)                                  = %s", buf);
	fixtoa(fix_val(1.0) - fix_div(a, (a + x)), buf, 6);
	printf("\n1.0 - a / (a + x)                            = %s", buf);
	fixtoa(fix_div(fix_val(1.0), (fix_div(a, x) + fix_val(1.0))), buf, 6);
	printf("\n1.0 / ( a / x + 1)                           = %s", buf);
	fixtoa(fix_div((fix_val(1.0) - fix_div((a + fix_mul(fix_val(0.1), x)), (a + x))), fix_val(0.9)), buf, 6);
	printf("\n(1.0 - (a + 0.1 * x) / (a + x)) / 0.9        = %s", buf);
	fixtoa(fix_div(a, (a + x)), buf, 6);
	printf("\na / ( a + x)                                 = %s", buf);
	fixtoa(fix_val(1.0) - fix_div(x, (a + x)), buf, 6);
	printf("\n1.0 - x / (a + x)                            = %s", buf);
	fixtoa(fix_div(fix_val(1.0), (fix_val(1.0) + fix_div(x, a))), buf, 6);
	printf("\n1.0 / (1 + x / a)                            = %s", buf);
	fixtoa(fix_div((a + fix_div(x, fix_val(10.0))), (a + x)) - fix_div(fix_mul(fix_val(0.1), x), (a + x)), buf, 6);
	printf("\n(a + x / 10.0) / (a + x) - 0.1 * x / (a + x) = %s", buf);
	fixtoa(fix_ln(fix_div((a + x), a)), buf, 6);
	printf("\nlog((a + x) / a))                            = %s", buf);
	fixtoa(fix_ln(a + x) - fix_ln(a), buf, 6);
	printf("\nlog(a + x) - log(a)                          = %s", buf);
	fixtoa(fix_ln(fix_val(1.0) + fix_div(x, a)), buf, 6);
	printf("\nlog(1.0 + x / a)                             = %s", buf);
	fixtoa(-fix_ln(fix_val(1.0) - fix_div(x, (a + x))), buf, 6);
	printf("\n-log(1.0 - x / (a + x))                      = %s", buf);
	printf("\n");
}

void testfp(void){
	int i;
	fixed_t a, x;
	char buf[30];

	fixtoa(epsilon(), buf, 6);
	printf("\nmachine epsilon: %s", buf);

	a = fix_div(fix_val(3.0), fix_val(4.0));
	for (i = 3; i > -7; i--){
		x = fix_pow(fix_val(3.0), fix_val(i));
		doit(a, x);
	}
}

void main(void){
	testfp();
	return;
}
