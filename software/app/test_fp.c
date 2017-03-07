#include <hf-risc.h>
#include <fp-math.h>

float epsilon(void){
	double x = 1.0;
	
	while ((1.0 + (x / 2.0)) > 1.0)
		x = x / 2.0;
	return x;
}

void doit(float a, float x){
	printf("\n\na = %f, x = %f\n", a, x);
	printf("\nx                                            = %.9f", x);
	printf("\n(a + x) - a                                  = %.9f", (a + x) - a);
	printf("\n4.0 * (a + x) - 3 * x - 3                    = %.9f", 4.0 * (a + x) - 3 * x - 3);
	printf("\n0.2 * (a + x) + 0.8 * x - 0.15               = %.9f", 0.2 * (a + x) + 0.8 * x - 0.15);
	printf("\nx / (a + x)                                  = %.9f", x / (a + x));
	printf("\n1.0 - a / (a + x)                            = %.9f", 1.0 - a / (a + x));
	printf("\n1.0 / ( a / x + 1)                           = %.9f", 1.0 / ( a / x + 1));
	printf("\n(1.0 - (a + 0.1 * x) / (a + x)) / 0.9        = %.9f", (1.0 - (a + 0.1 * x) / (a + x)) / 0.9);
	printf("\na / ( a + x)                                 = %.9f", a / ( a + x));
	printf("\n1.0 - x / (a + x)                            = %.9f", 1.0 - x / (a + x));
	printf("\n1.0 / (1 + x / a)                            = %.9f", 1.0 / (1 + x / a));
	printf("\n(a + x / 10.0) / (a + x) - 0.1 * x / (a + x) = %.9f", (a + x / 10.0) / (a + x) - 0.1 * x / (a + x));
	printf("\nlog((a + x) / a))                            = %.9f", log((a + x) / a));
	printf("\nlog(a + x) - log(a)                          = %.9f", log(a + x) - log(a));
	printf("\nlog(1.0 + x / a)                             = %.9f", log(1.0 + x / a));
	printf("\n-log(1.0 - x / (a + x))                      = %.9f", -log(1.0 - x / (a + x)));
	printf("\n");
}

void testfp(void){
	int i;
	float a, x;

	printf("\nmachine epsilon: %.9f", epsilon());

	a = 3.0 / 4.0;
	for (i = -10; i > -13; i--){
		x = pow(3.0, i);
		doit(a, x);
	}

}

void main(void){
	testfp();
	return;
}

