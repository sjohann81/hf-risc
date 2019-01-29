#include <hf-risc.h>
#include <math.h>

/*
f1(x) : x^6 - x^5 + 3x^4 - x^2 - 3 = 0
*/
float f1(float x){
	float val;

	val = pow(x, 6) - pow(x, 5) + 3 * pow(x, 4) - pow(x, 2) - 3;
	return val;
}

/*
fd1(x) : 6x^5 - 5x^4 + 12x^3 - 2x = 0
*/
float fd1(float x){
	float val;

	val = 6 * pow(x, 5) - 5 * pow(x, 4) + 12 * pow(x, 3) - 2 * x;
	return val;
}

/*
f2(x) : x^2 + sqrt(3x) - 6 = 0
if y = sqrt(x) then f2(y) = y^4 + y*sqrt(3) - 6
*/
float f2(float x){
	float val;

	val = pow(sqrt(x), 4) + sqrt(x)*sqrt(3) - 6;
	return val;
}

/*
fd2(x) : 4*y^3 + sqrt(3)
*/
float fd2(float x){
	float val;

	val = 4 * pow(sqrt(x), 3) + sqrt(3);
	return val;
}

int main(){
	int i;
	float x, rn = -1.0, rp = 1.0;
	int8_t buf[30];

	/*
	negative root: -0.947064
	positive root: 1.077812
	*/
	x = rn;
	for (i = 0; i < 10; i++)		/* newton-raphson, 10 iterations */
		x = x - f1(x) / fd1(x);
	ftoa(x, buf, 6);
	printf("\nnegative root: %s", buf);

	x = rp;
	for (i = 0; i < 10; i++)		/* newton-raphson, 10 iterations */
		x = x - f1(x) / fd1(x);
	ftoa(x, buf, 6);
	printf("\npositive root: %s", buf);

	/*
	positive root: 1.900522
	*/
	x = rp;
	for (i = 0; i < 20; i++)		/* newton-raphson, 20 iterations */
		x = x - f2(x) / fd2(x);
	ftoa(x, buf, 6);
	printf("\npositive root: %s", buf);

	return 0;
}

