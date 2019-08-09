#include <hf-risc.h>
#include <fixed.h>

fixed_t epsilon(void)
{
	fixed_t x = fix_val(1.0);

	while ((fix_val(1.0) + fix_div(x, fix_val(2.0))) > fix_val(1.0))
		x = fix_div(x, fix_val(2.0));

	return x;
}

void doit(fixed_t a, fixed_t x)
{
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

void testfp(void)
{
	int i;
	fixed_t a, x;
	char buf[30];

	fixtoa(epsilon(), buf, 6);
	printf("\nmachine epsilon: %s", buf);

	a = fix_div(fix_val(3.0), fix_val(4.0));
	for (i = 3; i > -7; i--) {
		x = fix_pow(fix_val(3.0), fix_val(i));
		doit(a, x);
	}

}

int testmath()
{
	int8_t buf[30], buf2[30];
	float angle, degrees[] = {0.0, 30.0, 45.0, 60.0, 90.0, 120.0, 135.0, 150.0, 180.0, 210.0, 225.0, 240.0, 270.0, 300.0, 315.0, 330.0, 360.0};
	int32_t i, angles = sizeof(degrees) / sizeof(float);

	fixtoa(fix_div(fix_val(355.0), fix_val(113.0)), buf, 6);
	printf("\naprox. pi: %s", buf);
	fixtoa(fix_mul(fix_div(fix_val(1.0), fix_val(9.0)), fix_val(9.0)), buf, 6);
	printf("\n(1.0 / 9.0) * 9.0: %s\n", buf);
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

	for (i = 0; i < angles; i++) {
		angle = degrees[i];
		ftoa(angle, buf2, 2);

		fixtoa(fix_sin(fix_rad(fix_val(angle))), buf, 6);
		printf("\nsin %s: %s ", buf2, buf);
		fixtoa(fix_cos(fix_rad(fix_val(angle))), buf, 6);
		printf("cos %s: %s ", buf2, buf);
		fixtoa(fix_tan(fix_rad(fix_val(angle))), buf, 6);
		printf("tan %s: %s\n", buf2, buf);

		fixtoa(fix_asin(fix_rad(fix_val(angle))), buf, 6);
		printf("asin %s: %s ", buf2, buf);
		fixtoa(fix_acos(fix_rad(fix_val(angle))), buf, 6);
		printf("acos %s: %s ", buf2, buf);
		fixtoa(fix_atan(fix_rad(fix_val(angle))), buf, 6);
		printf("atan %s: %s\n", buf2, buf);

		fixtoa(fix_sinh(fix_rad(fix_val(angle))), buf, 6);
		printf("sinh %s: %s ", buf2, buf);
		fixtoa(fix_cosh(fix_rad(fix_val(angle))), buf, 6);
		printf("cosh %s: %s ", buf2, buf);
		fixtoa(fix_tanh(fix_rad(fix_val(angle))), buf, 6);
		printf("tanh %s: %s\n", buf2, buf);
	}

	return 0;
}

void testconv(void)
{
	int i;
	fixed_t val_fix[5];
	float val_float[5] = {-225.12345, -99.555, 0.0, 99.666, 225.12345};
	float val_float2[5];
	char buf[30];

	for (i = 0; i < 5; i++) {
		val_fix[i] = float_to_fix(val_float[i]);
		val_float2[i] = fix_to_float(val_fix[i]);
	}

	printf("float values:\n");
	for (i = 0; i < 5; i++) {
		ftoa(val_float[i], buf, 6);
		printf("[%s] ", buf);
	}
	printf("\nfloat_to_fix() values:\n");
	for (i = 0; i < 5; i++) {
		fixtoa(val_fix[i], buf, 6);
		printf("[%s] ", buf);
	}
	printf("\nfix_to_float() values:\n");
	for (i = 0; i < 5; i++) {
		ftoa(val_float2[i], buf, 6);
		printf("[%s] ", buf);
	}
}

int main(void)
{
	testfp();
	testmath();
	testconv();

	return 0;
}
