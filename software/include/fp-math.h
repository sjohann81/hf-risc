/*
floating point arithmetic library
*/

#define PI		3.1415926535897932384626433f
#define PI_2		(PI / 2.0f)
#define PI_4		(PI / 4.0f)
#define PI2		(PI * 2.0f)
#define ONE_PI		(1.0f / PI)
#define TWO_PI		(2.0f / PI)
#define E_		2.7182818284590452353602874f
#define	LOG2E		1.4426950408889634073599247f
#define LOG10E		0.43429448190325182765f
#define SQRT2		1.4142135623730950488016887f
#define TWO_SQRTPI	1.12837916709551257390f
#define	LN2		0.69314718055994530942f
#define LN10		2.30258509299404568402f
#define SQRT1_2		0.707106781186547524f

#define HUGE		1.701411733192644270e38f
#define LOGHUGE		39

extern float fabs(float n);
extern float frexp(float value, int32_t *eptr);
extern float ldexp(float x, int32_t exp);
extern float modf(float x, float *iptr);
extern float floor(float d);
extern float ceil(float d);
extern float sqrt(float arg);
extern float exp(float arg);
extern float log(float arg);
extern float log10(float arg);
extern float pow(float arg1, float arg2);
extern float sin(float rad);
extern float cos(float rad);
extern float tan(float rad);
extern float atan(float arg);
extern float atan2(float arg1, float arg2);
extern float asin(float arg);
extern float acos(float arg);
extern float sinh(float arg);
extern float cosh(float arg);
extern float tanh(float arg);

