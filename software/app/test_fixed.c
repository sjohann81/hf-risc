#include <hf-risc.h>
#define FIX_IBITS		16
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
	float f;
	char buf[30], buf2[30];

	fixtoa(epsilon(), buf, 6);
	printf("\nmachine epsilon: %s", buf);
	
	a = fix_div(fix_val(1.0), fix_val(137.0));
	f = 1.0f / 137.0f;
	fixtoa(a, buf, 10);
	ftoa(f, buf2, 10);
	printf("\n1 / 137: %s (fix) %s (float)", buf, buf2);

	a = fix_div(fix_val(3.0), fix_val(4.0));
	x = fix_val(0.004115226337);	/* 3**-5 */
	doit(a, x);
	
//	for (i = 3; i > -7; i--) {
//		x = fix_pow(fix_val(3.0), fix_val(i));
//		doit(a, x);
//	}

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

	printf("\nfloat values:\n");
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

/*
testmul() computes Z = X * Y

        X             Y               Z
                                      (X * Y)
  -56.31634060   91.26351530    -5139.62721241
   65.90184679   12.33908855      813.16872333
  -16.93858370  -86.77625302     1469.86682531
  -48.48444152  -78.00864129     3782.20540713
  -91.23420044   26.79314246    -2444.45092977
  -87.65455419  -10.09220794      884.62798762
  -19.73874370   50.93469728    -1005.38693516
   59.45739082  -99.63232577    -5923.87813164
   79.50081218  -29.84953240    -2373.06206930
  -81.09104996  -97.27662168     7888.26338863
   71.81937106   68.16949013     4895.88990642
  -75.37921685  -98.49752719     7424.66646060
  -47.93940045   82.49674140    -3954.84432133
  -77.26719071  -29.67426802     2292.84732627
   64.57746334  -46.57354595    -3007.60145594
   38.41329996   12.33249498      473.73182872
   72.24315812   -9.24124500     -667.61672363
   82.39540568   19.58337543     1613.58016298
  -62.20906180   52.29841123    -3253.43509616
  -20.60230483  -62.93717658     1296.65089702
*/
void testmul(void)
{
	char buf1[30];
	char buf2[30];
	char buf3[30];
	char buf4[30];
	
	float x[20] = {
		-56.31634060, 65.90184679, -16.93858370, -48.48444152,
		-91.23420044, -87.65455419, -19.73874370, 59.45739082,
		79.50081218, -81.09104996, 71.81937106, -75.37921685,
		-47.93940045, -77.26719071, 64.57746334, 38.41329996,
		72.24315812, 82.39540568, -62.20906180, -20.60230483
	};
	float y[20] = {
		91.26351530, 12.33908855, -86.77625302, -78.00864129,
		26.79314246, -10.09220794, 50.93469728, -99.63232577,
		-29.84953240, -97.27662168, 68.16949013, -98.49752719,
		82.49674140, -29.67426802, -46.57354595, 12.33249498,
		-9.24124500, 19.58337543, 52.29841123, -62.93717658
	};
	
	printf("\n\nx		y		z(float)	z(fixed)\n");
	for (int i = 0; i < 20; i++) {
		ftoa(x[i], buf1, 8);
		ftoa(y[i], buf2, 8);
		ftoa((x[i] * y[i]), buf3, 8);
		fixtoa(fix_mul(float_to_fix(x[i]), float_to_fix(y[i])), buf4, 8);
		printf("%s	%s	%s	%s\n", buf1, buf2, buf3, buf4);
	}
}

int main(void)
{
	testfp();
	testmath();
	testconv();
	testmul();

	return 0;
}
