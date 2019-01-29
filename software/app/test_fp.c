#include <hf-risc.h>
#include <math.h>

float epsilon(void){
	float x = 1.0f;
	
	while ((1.0f + (x / 2.0f)) > 1.0f)
		x = x / 2.0f;
		
	return x;
}

void doit(float a, float x){
	char buf[30], buf2[30];
	
	ftoa(a, buf, 6); ftoa(x, buf2, 6);				printf("\n\na = %s, x = %s\n", buf , buf2);
	ftoa(x, buf, 6);						printf("\nx                                            = %s", buf);
	ftoa((a + x) - a, buf, 6);					printf("\n(a + x) - a                                  = %s", buf);
	ftoa(4.0f * (a + x) - 3.0f * x - 3.0f, buf, 6);			printf("\n4.0 * (a + x) - 3 * x - 3                    = %s", buf);
	ftoa(0.2f * (a + x) + 0.8f * x - 0.15f, buf, 6);		printf("\n0.2 * (a + x) + 0.8 * x - 0.15               = %s", buf);
	ftoa(x / (a + x), buf, 6);					printf("\nx / (a + x)                                  = %s", buf);
	ftoa(1.0f - a / (a + x), buf, 6);				printf("\n1.0 - a / (a + x)                            = %s", buf);
	ftoa(1.0f / ( a / x + 1), buf, 6);				printf("\n1.0 / ( a / x + 1)                           = %s", buf);
	ftoa((1.0f - (a + 0.1f * x) / (a + x)) / 0.9f, buf, 6);		printf("\n(1.0 - (a + 0.1 * x) / (a + x)) / 0.9        = %s", buf);
	ftoa(a / ( a + x), buf, 6);					printf("\na / ( a + x)                                 = %s", buf);
	ftoa(1.0f - x / (a + x), buf, 6);				printf("\n1.0 - x / (a + x)                            = %s", buf);
	ftoa(1.0f / (1.0f + x / a), buf, 6);				printf("\n1.0 / (1 + x / a)                            = %s", buf);
	ftoa((a + x / 10.0f) / (a + x) - 0.1f * x / (a + x), buf, 6);	printf("\n(a + x / 10.0) / (a + x) - 0.1 * x / (a + x) = %s", buf);
	ftoa(log((a + x) / a), buf, 6);					printf("\nlog((a + x) / a))                            = %s", buf);
	ftoa(log(a + x) - log(a), buf, 6);				printf("\nlog(a + x) - log(a)                          = %s", buf);
	ftoa(log(1.0f + x / a), buf, 6);				printf("\nlog(1.0 + x / a)                             = %s", buf);
	ftoa(-log(1.0f - x / (a + x)), buf, 6);				printf("\n-log(1.0 - x / (a + x))                      = %s", buf);
	printf("\n");
}

void testfp(void){
	int i;
	float a, x;
	char buf[30];

	ftoa(epsilon(), buf, 6);
	printf("\nmachine epsilon: %s", buf);

	a = 3.0f / 4.0f;
	for (i = 3; i > -7; i--){
		x = pow(3.0f, i);
		doit(a, x);
	}
}

void main(void){
	testfp();
	return;
}
