#include <hf-risc.h>
#include <math.h>

float epsilon(void){
	float x = 1.0f;
	
	while ((1.0f + (x / 2.0f)) > 1.0f)
		x = x / 2.0f;
		
	return x;
}

double depsilon(void){
	double x = 1.0;
	
	while ((1.0 + (x / 2.0)) > 1.0)
		x = x / 2.0;
		
	return x;
}

void doit(float a, float x){
	char buf[30], buf2[30];
	float val;
	
	ftoa(a, buf, 6); ftoa(x, buf2, 6);
	printf("\n\na = %s, x = %s\n", buf , buf2);
	ftoa(x, buf, 6);
	printf("\nx                                            = %s", buf);
	
	val = (a + x) - a;
	ftoa(val, buf, 6);
	printf("\n(a + x) - a                                  = %s", buf);
	
	val = 4.0f * (a + x) - 3.0f * x - 3.0f; 
	ftoa(val, buf, 6);
	printf("\n4.0 * (a + x) - 3 * x - 3                    = %s", buf);
	
	val = 0.2f * (a + x) + 0.8f * x - 0.15f;
	ftoa(val, buf, 6);
	printf("\n0.2 * (a + x) + 0.8 * x - 0.15               = %s", buf);
	
	val = x / (a + x);
	ftoa(val, buf, 6);
	printf("\nx / (a + x)                                  = %s", buf);
	
	val = 1.0f - a / (a + x);
	ftoa(val, buf, 6);
	printf("\n1.0 - a / (a + x)                            = %s", buf);
	
	val = 1.0f / (a / x + 1);
	ftoa(val, buf, 6);
	printf("\n1.0 / (a / x + 1)                            = %s", buf);
	
	val = (1.0f - (a + 0.1f * x) / (a + x)) / 0.9f; 
	ftoa(val , buf, 6);
	printf("\n(1.0 - (a + 0.1 * x) / (a + x)) / 0.9        = %s", buf);
	
	val = a / (a + x);
	ftoa(val, buf, 6);
	printf("\na / (a + x)                                  = %s", buf);
	
	val = 1.0f - x / (a + x);
	ftoa(val, buf, 6);
	printf("\n1.0 - x / (a + x)                            = %s", buf);
	
	val = 1.0f / (1.0f + x / a);
	ftoa(val, buf, 6);
	printf("\n1.0 / (1 + x / a)                            = %s", buf);
	
	val = (a + x / 10.0f) / (a + x) - 0.1f * x / (a + x);
	ftoa(val, buf, 6);
	printf("\n(a + x / 10.0) / (a + x) - 0.1 * x / (a + x) = %s", buf);
	
	val = log((a + x) / a); 
	ftoa(val, buf, 6);
	printf("\nlog((a + x) / a))                            = %s", buf);
	
	val = log(a + x) - log(a);
	ftoa(val, buf, 6);
	printf("\nlog(a + x) - log(a)                          = %s", buf);
	
	val = log(1.0f + x / a);
	ftoa(val, buf, 6);
	printf("\nlog(1.0 + x / a)                             = %s", buf);
	
	val = -log(1.0f - x / (a + x));
	ftoa(val, buf, 6);
	printf("\n-log(1.0 - x / (a + x))                      = %s", buf);
	printf("\n");
}

void doit2(float a, float x){
	char buf[30], buf2[30];
	double val;
	
	ftoa(a, buf, 6); ftoa(x, buf2, 6);
	printf("\n\na = %s, x = %s\n", buf , buf2);
	ftoa(x, buf, 6);
	printf("\nx                                            = %s", buf);
	
	val = (a + x) - a;
	ftoa(val, buf, 6);
	printf("\n(a + x) - a                                  = %s", buf);
	
	val = 4.0 * (a + x) - 3.0 * x - 3.0; 
	ftoa(val, buf, 6);
	printf("\n4.0 * (a + x) - 3 * x - 3                    = %s", buf);
	
	val = 0.2 * (a + x) + 0.8 * x - 0.15;
	ftoa(val, buf, 6);
	printf("\n0.2 * (a + x) + 0.8 * x - 0.15               = %s", buf);
	
	val = x / (a + x);
	ftoa(val, buf, 6);
	printf("\nx / (a + x)                                  = %s", buf);
	
	val = 1.0 - a / (a + x);
	ftoa(val, buf, 6);
	printf("\n1.0 - a / (a + x)                            = %s", buf);
	
	val = 1.0 / (a / x + 1);
	ftoa(val, buf, 6);
	printf("\n1.0 / (a / x + 1)                            = %s", buf);
	
	val = (1.0 - (a + 0.1f * x) / (a + x)) / 0.9; 
	ftoa(val , buf, 6);
	printf("\n(1.0 - (a + 0.1 * x) / (a + x)) / 0.9        = %s", buf);
	
	val = a / (a + x);
	ftoa(val, buf, 6);
	printf("\na / (a + x)                                  = %s", buf);
	
	val = 1.0 - x / (a + x);
	ftoa(val, buf, 6);
	printf("\n1.0 - x / (a + x)                            = %s", buf);
	
	val = 1.0 / (1.0 + x / a);
	ftoa(val, buf, 6);
	printf("\n1.0 / (1 + x / a)                            = %s", buf);
	
	val = (a + x / 10.0) / (a + x) - 0.1 * x / (a + x);
	ftoa(val, buf, 6);
	printf("\n(a + x / 10.0) / (a + x) - 0.1 * x / (a + x) = %s", buf);
	
	val = log((a + x) / a); 
	ftoa(val, buf, 6);
	printf("\nlog((a + x) / a))                            = %s", buf);
	
	val = log(a + x) - log(a);
	ftoa(val, buf, 6);
	printf("\nlog(a + x) - log(a)                          = %s", buf);
	
	val = log(1.0 + x / a);
	ftoa(val, buf, 6);
	printf("\nlog(1.0 + x / a)                             = %s", buf);
	
	val = -log(1.0 - x / (a + x));
	ftoa(val, buf, 6);
	printf("\n-log(1.0 - x / (a + x))                      = %s", buf);
	printf("\n");
}

void testfp(void){
	int i;
	float a, x;
	char buf[30];

	ftoa(epsilon(), buf, 12);
	printf("\nmachine epsilon (float): %s", buf);
	
	a = 3.0f / 4.0f;
	for (i = 3; i > -7; i--){
		x = pow(3.0f, i);
		doit(a, x);
	}
}

void testdp(void){
	int i;
	float a, x;
	char buf[30];

	ftoa(depsilon(), buf, 12);
	printf("\nmachine epsilon (double): %s", buf);

	a = 3.0 / 4.0;
	for (i = 3; i > -7; i--){
		x = pow(3.0, i);
		doit2(a, x);
	}
}

void main(void){
	printf("\nFP test\n");
	printf("=======\n");
	testfp();
	printf("\nDP test\n");
	printf("=======\n");
	testdp();
	return;
}
