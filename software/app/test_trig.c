#include <hf-risc.h>
#include <math.h>

int main() {
	float pi = 4 * atan(1);
	float degrees[] = {0.0, 30.0, 45.0, 60.0, 90.0, 120.0, 135.0, 150.0, 180.0, 210.0, 225.0, 240.0, 270.0, 300.0, 315.0, 330.0, 360.0};
	float radians;
	int i;
	char buf[30];
	

	for (i = 0; i < sizeof(degrees) / sizeof(float); i++){
		radians = degrees[i] * pi / 180;
		printf("\nAngle is %d degrees.\n", (int)degrees[i]);
		/*sine*/
		ftoa(sin(radians), buf, 6);
		printf("sin: %s\n", buf);
		/*cosine*/
		ftoa(cos(radians), buf, 6);
		printf("cos: %s\n", buf);
		/*tangent*/
		ftoa(tan(radians), buf, 6);
		printf("tan: %s\n", buf);
		/*arcsine*/
		ftoa(asin(sin(radians)), buf, 6);
		printf("asin(sin): %s\n", buf);
		/*arccosine*/
		ftoa(acos(cos(radians)), buf, 6);
		printf("acos(cos): %s\n", buf);
		/*arctangent*/
		ftoa(atan(tan(radians)), buf, 6);
		printf("atan(tan): %s\n", buf);
	}
	return 0;
}
