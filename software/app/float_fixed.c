#include <hf-risc.h>
#define FIX_IBITS		12
#include <fixed.h>

float testfloat_add(float min, float max, float step)
{
	float i, s = 0.0;
	
	for (i = min; i < max; i += step)
		s += step;
	
	return s;
}

fixed_t testfixed_add(fixed_t min, fixed_t max, fixed_t step)
{
	fixed_t i, s = fix_val(0.0);
	
	for (i = min; i < max; i += step)
		s += step;
	
	return s;
}

float testfloat_sub(float min, float max, float step)
{
	float i, s = 0.0;
	
	for (i = max; i >= min; i -= step)
		s -= step;
	
	return s;
}

fixed_t testfixed_sub(fixed_t min, fixed_t max, fixed_t step)
{
	fixed_t i, s = fix_val(0.0);
	
	for (i = max; i >= min; i -= step)
		s -= step;
	
	return s;
}

float testfloat_mul(float min, float max, float step)
{
	float i, s = max;
	
	for (i = min; i < max; i += step)
		s *= 1.000001;
	
	return s;
}

fixed_t testfixed_mul(fixed_t min, fixed_t max, fixed_t step)
{
	fixed_t i, s = max;
	
	for (i = min; i < max; i += step)
		s = fix_mul(s, fix_val(1.000001));
	
	return s;
}

float testfloat_div(float min, float max, float step)
{
	float i, s = max;
	
	for (i = min; i < max; i += step)
		s /= 1.000001;
	
	return s;
}

fixed_t testfixed_div(fixed_t min, fixed_t max, fixed_t step)
{
	fixed_t i, s = max;
	
	for (i = min; i < max; i += step)
		s = fix_div(s, fix_val(1.000001));
	
	return s;
}

int main()
{
	/* volatile is being used to avoid the optimizer from being too smart */
	volatile float minfl = -1000.0;
	volatile float maxfl = 1000.0;
	volatile float stepfl = 0.1;
	float vfl;
	volatile fixed_t minfx = fix_val(minfl);
	volatile fixed_t maxfx = fix_val(maxfl);
	volatile fixed_t stepfx = fix_val(stepfl);
	fixed_t vfx;
	uint32_t cycles;
	int8_t buf[20];
	
	printf("\nfloat / fixed point speed test\n");
	
	cycles = TIMER0;
	vfl = testfloat_add(minfl, maxfl, stepfl);
	cycles = TIMER0 - cycles;
	ftoa(vfl, buf, 6);
	printf("float add: %d cycles - %s\n", cycles, buf);

	cycles = TIMER0;
	vfx = testfixed_add(minfx, maxfx, stepfx);
	cycles = TIMER0 - cycles;
	fixtoa(vfx, buf, 6);
	printf("fixed add: %d cycles - %s\n", cycles, buf);
	
	cycles = TIMER0;
	vfl = testfloat_sub(minfl, maxfl, stepfl);
	cycles = TIMER0 - cycles;
	ftoa(vfl, buf, 6);
	printf("float sub: %d cycles - %s\n", cycles, buf);

	cycles = TIMER0;
	vfx = testfixed_sub(minfx, maxfx, stepfx);
	cycles = TIMER0 - cycles;
	fixtoa(vfx, buf, 6);
	printf("fixed sub: %d cycles - %s\n", cycles, buf);

	cycles = TIMER0;
	vfl = testfloat_mul(minfl, maxfl, stepfl);
	cycles = TIMER0 - cycles;
	ftoa(vfl, buf, 6);
	printf("float mul: %d cycles - %s\n", cycles, buf);

	cycles = TIMER0;
	vfx = testfixed_mul(minfx, maxfx, stepfx);
	cycles = TIMER0 - cycles;
	fixtoa(vfx, buf, 6);
	printf("fixed mul: %d cycles - %s\n", cycles, buf);

	cycles = TIMER0;
	vfl = testfloat_div(minfl, maxfl, stepfl);
	cycles = TIMER0 - cycles;
	ftoa(vfl, buf, 6);
	printf("float div: %d cycles - %s\n", cycles, buf);

	cycles = TIMER0;
	vfx = testfixed_div(minfx, maxfx, stepfx);
	cycles = TIMER0 - cycles;
	fixtoa(vfx, buf, 6);
	printf("fixed div: %d cycles - %s\n", cycles, buf);

	return 0;
}
