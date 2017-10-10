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

float fabs(float n);
float frexp(float value, int32_t *eptr);
float ldexp(float x, int32_t exp);
float modf(float x, float *iptr);
float floor(float d);
float ceil(float d);
float sqrt(float arg);
float exp(float arg);
float log(float arg);
float log10(float arg);
float pow(float arg1, float arg2);
float sin(float rad);
float cos(float rad);
float tan(float rad);
float atan(float arg);
float atan2(float arg1, float arg2);
float asin(float arg);
float acos(float arg);
float sinh(float arg);
float cosh(float arg);
float tanh(float arg);
