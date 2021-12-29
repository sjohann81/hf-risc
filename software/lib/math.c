/*
floating point math implementation

functions implemented:
fabs(), frexp(), ldexp(), modf(), floor(), ceil(), sin(), cos(), tan()
sqrt(), exp(), log(), log10(), pow(), atan(), atan2(), asin(), acos(), sinh(), cosh(), tanh()

some functions adapted from original January 1979 Unix V7 sources (for PDP-11/45).
http://www.bsdlover.cn/study/UnixTree/V7/index.html
modifications include:
-double-precision to single-precision conversion
-no support to range / domain checking (was present on the original implementation)
*/

#include <hf-risc.h>
#include <math.h>

// absolute value of a floating point number
float fabs(float n){
	return n >= 0.0f ? n : -n;
}

// frexp: returns the mantissa and exponent parts
float frexp(float value, int32_t *eptr){
	union{
                float v;
                struct{
#ifdef LITTLE_ENDIAN
			uint32_t u_mant	: 23;
			uint32_t u_exp	: 8;
			uint32_t u_sign	: 1;
#else
			uint32_t u_sign	: 1;
			uint32_t u_exp	: 8;
			uint32_t u_mant	: 23;
#endif
                } s;
        } u;

	if (value){
		u.v = value;
		if (u.s.u_exp != SNG_EXP_INFNAN){
			*eptr = u.s.u_exp - (SNG_EXP_BIAS - 1);
			u.s.u_exp = SNG_EXP_BIAS - 1;
		}
		return (u.v);
	}else{
		*eptr = 0;
		return (0.0f);
	}
}

// ldexp: x * 2^exp
float ldexp(float val, int expon){
	int32_t oldexp, newexp;
	union{
                float v;
                struct{
#ifdef LITTLE_ENDIAN
			uint32_t u_mant	: 23;
			uint32_t u_exp	: 8;
			uint32_t u_sign	: 1;
#else
			uint32_t u_sign	: 1;
			uint32_t u_exp	: 8;
			uint32_t u_mant	: 23;
#endif
                } s;
        } u, mul;

	u.v = val;
	oldexp = u.s.u_exp;

	/*
	 * If input is zero, Inf or NaN, just return it.
	 */
	if (u.v == 0.0 || oldexp == SNG_EXP_INFNAN)
		return (val);

	if (oldexp == 0) {
		/*
		 * u.v is denormal.  We must adjust it so that the exponent
		 * arithmetic below will work.
		 */
		if (expon <= SNG_EXP_BIAS) {
			/*
			 * Optimization: if the scaling can be done in a single
			 * multiply, or underflows, just do it now.
			 */
			if (expon <= -SNG_FRACBITS) {
				return (val < 0.0 ? -0.0 : 0.0);
			}
			mul.v = 0.0;
			mul.s.u_exp = expon + SNG_EXP_BIAS;
			u.v *= mul.v;
			if (u.v == 0.0) {
				return (val < 0.0 ? -0.0 : 0.0);
			}
			return (u.v);
		} else {
			/*
			 * We know that expon is very large, and therefore the
			 * result cannot be denormal (though it may be Inf).
			 * Shift u.v by just enough to make it normal.
			 */
			mul.v = 0.0;
			mul.s.u_exp = SNG_FRACBITS + SNG_EXP_BIAS;
			u.v *= mul.v;
			expon -= SNG_FRACBITS;
			oldexp = u.s.u_exp;
		}
	}

	/*
	 * u.v is now normalized and oldexp has been adjusted if necessary.
	 * Calculate the new exponent and check for underflow and overflow.
	 */
	newexp = oldexp + expon;

	if (newexp <= 0) {
		/*
		 * The output number is either denormal or underflows (see
		 * comments in machine/ieee.h).
		 */
		if (newexp <= -SNG_FRACBITS) {
			return (val < 0.0 ? -0.0 : 0.0);
		}
		/*
		 * Denormalize the result.  We do this with a multiply.  If
		 * expon is very large, it won't fit in a double, so we have
		 * to adjust the exponent first.  This is safe because we know
		 * that u.v is normal at this point.
		 */
		if (expon <= -SNG_EXP_BIAS) {
			u.s.u_exp = 1;
			expon += oldexp - 1;
		}
		mul.v = 0.0;
		mul.s.u_exp = expon + SNG_EXP_BIAS;
		u.v *= mul.v;
		return (u.v);
	} else if (newexp >= SNG_EXP_INFNAN) {
		/*
		 * The result overflowed; return +/-Inf.
		 */
		u.s.u_exp = SNG_EXP_INFNAN;
		u.s.u_mant = 0;
		return (u.v);
	} else {
		/*
		 * The result is normal; just replace the old exponent with the
		 * new one.
		 */
		u.s.u_exp = newexp;
		return (u.v);
	}
}


// returns the integer and fractional parts of a floating point number
float modf(float val, float *iptr){
	union{
                float v;
                struct{
#ifdef LITTLE_ENDIAN
			uint32_t u_mant	: 23;
			uint32_t u_exp	: 8;
			uint32_t u_sign	: 1;
#else
			uint32_t u_sign	: 1;
			uint32_t u_exp	: 8;
			uint32_t u_mant	: 23;
#endif
                } s;
        } u, v;
	uint32_t frac;

	/*
	 * If input is Inf or NaN, return it and leave i alone.
	 */
	u.v = val;
	if (u.s.u_exp == SNG_EXP_INFNAN)
		return (u.v);

	/*
	 * If input can't have a fractional part, return
	 * (appropriately signed) zero, and make i be the input.
	 */
	if ((int32_t)u.s.u_exp - SNG_EXP_BIAS > SNG_FRACBITS - 1) {
		*iptr = u.v;
		v.v = 0.0;
		v.s.u_sign = u.s.u_sign;
		return (v.v);
	}

	/*
	 * If |input| < 1.0, return it, and set i to the appropriately
	 * signed zero.
	 */
	if (u.s.u_exp < SNG_EXP_BIAS) {
		v.v = 0.0;
		v.s.u_sign = u.s.u_sign;
		*iptr = v.v;
		return (u.v);
	}

	/*
	 * There can be a fractional part of the input.
	 * If you look at the math involved for a few seconds, it's
	 * plain to see that the integral part is the input, with the
	 * low (DBL_FRACBITS - (exponent - DBL_EXP_BIAS)) bits zeroed,
	 * the the fractional part is the part with the rest of the
	 * bits zeroed.  Just zeroing the high bits to get the
	 * fractional part would yield a fraction in need of
	 * normalization.  Therefore, we take the easy way out, and
	 * just use subtraction to get the fractional part.
	 */
	v.v = u.v;
	/* Zero the low bits of the fraction, the sleazy way. */
	frac = (uint32_t)v.s.u_mant;
	frac >>= SNG_FRACBITS - (u.s.u_exp - SNG_EXP_BIAS);
	frac <<= SNG_FRACBITS - (u.s.u_exp - SNG_EXP_BIAS);
	v.s.u_mant = frac;
	*iptr = v.v;

	u.v -= v.v;
	u.s.u_sign = v.s.u_sign;
	return (u.v);
}

// floor of a floating point number
float floor(float d){
	float fract;

	if (d<0.0f){
		d = -d;
		fract = modf(d, &d);
		if (fract != 0.0f)
			d += 1.0f;
		d = -d;
	}else{
		modf(d, &d);
	}
	return (d);
}

// ceil of a floating point number
float ceil(float d){
	return (-floor(-d));
}

// square root (newton's method of sucessive aproximations)
float sqrt(float arg){
	float x, temp;
	int32_t exp = 0;
	int32_t i;

	if(arg <= 0.0f){
		return(0.0f);
	}
	x = frexp(arg,&exp);
	while(x < 0.5f){
		x *= 2.0f;
		exp--;
	}
	if(exp & 1){
		x *= 2.0f;
		exp--;
	}
	temp = 0.5f*(1.0f+x);

	while(exp > 60){
		temp *= (1L<<30);
		exp -= 60;
	}
	while(exp < -60){
		temp /= (1L<<30);
		exp += 60;
	}
	if(exp >= 0)
		temp *= 1L << (exp/2);
	else
		temp /= 1L << (-exp/2);
	for(i=0; i<=4; i++)
		temp = 0.5f*(temp + arg/temp);
	return(temp);
}

// exponential
float exp(float arg){
	static float p0	= 0.2080384346694663001443843411e7f;
	static float p1	= 0.3028697169744036299076048876e5f;
	static float p2	= 0.6061485330061080841615584556e2f;
	static float q0	= 0.6002720360238832528230907598e7f;
	static float q1	= 0.3277251518082914423057964422e6f;
	static float q2	= 0.1749287689093076403844945335e4f;

	float fract;
	float temp1, temp2, xsq;
	int32_t ent;

	if(arg == 0.0f)
		return(1.0f);
	if(arg < -10000.0f)
		return(0.0f);
	if(arg > 10000.0f)
		return(HUGE);

	arg *= LOG2E;
	ent = floor(arg);
	fract = (arg - ent) - 0.5f;
	xsq = fract*fract;
	temp1 = ((p2*xsq+p1)*xsq+p0)*fract;
	temp2 = ((1.0f*xsq+q2)*xsq+q1)*xsq + q0;
	return(ldexp(SQRT2*(temp2+temp1)/(temp2-temp1), ent));
}

// natural logarithm
float log(float arg){
	static float p0	= -0.240139179559210510e2f;
	static float p1	=  0.309572928215376501e2f;
	static float p2	= -0.963769093368686593e1f;
	static float p3	=  0.421087371217979714e0f;
	static float q0	= -0.120069589779605255e2f;
	static float q1	=  0.194809660700889731e2f;
	static float q2	= -0.891110902798312337e1f;

	float x,z, zsq, temp;
	int32_t exp = 0;

	if(arg <= 0.0f){
		return(-HUGE);
	}
	x = frexp(arg,&exp);
	while(x < 0.5f){
		x *= 2.0f;
		exp--;
	}
	if(x < SQRT1_2){
		x *= 2.0f;
		exp--;
	}

	z = (x-1.0f)/(x+1.0f);
	zsq = z*z;

	temp = ((p3*zsq + p2)*zsq + p1)*zsq + p0;
	temp = temp/(((1.0f*zsq + q2)*zsq + q1)*zsq + q0);
	temp = temp*z + exp*LN2;
	return(temp);
}

// log base 10
float log10(float arg){
	return(log(arg)/LN10);
}

// pow()
float pow(float arg1, float arg2){
	float temp;
	int32_t l;

	if(arg1 <= 0.0f){
		if((arg1 == 0.0f) || (arg2 <= 0.0f))
			return(0.0f);

		l = (int32_t)arg2;
		if (l != arg2)
			return(0.0f);
		temp = exp(arg2 * log(-arg1));
		if(l & 1)
			temp = -temp;
		return(temp);
	}
	return(exp(arg2 * log(arg1)));
}

static float sinus(float arg, int quad){
	static float p0	=  0.1357884097877375669092680e8f;
	static float p1	= -0.4942908100902844161158627e7f;
	static float p2	=  0.4401030535375266501944918e6f;
	static float p3	= -0.1384727249982452873054457e5f;
	static float p4	=  0.1459688406665768722226959e3f;
	static float q0	=  0.8644558652922534429915149e7f;
	static float q1	=  0.4081792252343299749395779e6f;
	static float q2	=  0.9463096101538208180571257e4f;
	static float q3	=  0.1326534908786136358911494e3f;
	float e, f;
	float ysq;
	float x,y;
	int k;
	float temp1, temp2;

	x = arg;
	if(x<0) {
		x = -x;
		quad = quad + 2;
	}
	x = x * TWO_PI;	/*underflow?*/
	if(x>32764){
		y = modf(x,&e);
		e = e + quad;
		modf(0.25f*e,&f);
		quad = e - 4*f;
	}else{
		k = x;
		y = x - k;
		quad = (quad + k) & 03;
	}
	if (quad & 01)
		y = 1-y;
	if(quad > 1)
		y = -y;

	ysq = y*y;
	temp1 = ((((p4*ysq+p3)*ysq+p2)*ysq+p1)*ysq+p0)*y;
	temp2 = ((((ysq+q3)*ysq+q2)*ysq+q1)*ysq+q0);
	return(temp1/temp2);
}

float cos(float arg){
	if(arg<0)
		arg = -arg;

	return(sinus(arg, 1));
}

float sin(float arg){
	return(sinus(arg, 0));
}

float tan(float arg){
	static float invpi	= 1.27323954473516268f;
	static float p0		= -0.1306820264754825668269611177e+5f;
	static float p1		= 0.1055970901714953193602353981e+4f;
	static float p2		= -0.1550685653483266376941705728e+2f;
	static float p3		= 0.3422554387241003435328470489e-1f;
	static float p4		= 0.3386638642677172096076369e-4f;
	static float q0		= -0.1663895238947119001851464661e+5f;
	static float q1		= 0.4765751362916483698926655581e+4f;
	static float q2		= -0.1555033164031709966900124574e+3f;
	float sign, temp, e, x, xsq;
	int flag, i;

	flag = 0;
	sign = 1.0f;
	if(arg < 0.0f){
		arg = -arg;
		sign = -1.0f;
	}
	arg = arg*invpi;   /*overflow?*/
	x = modf(arg,&e);
	i = e;
	switch(i%4){
		case 1:
			x = 1.0f - x;
			flag = 1;
			break;

		case 2:
			sign = - sign;
			flag = 1;
			break;

		case 3:
			x = 1.0f - x;
			sign = - sign;
			break;

		case 0:
			break;
	}

	xsq = x*x;
	temp = ((((p4*xsq+p3)*xsq+p2)*xsq+p1)*xsq+p0)*x;
	temp = temp/(((1.0*xsq+q2)*xsq+q1)*xsq+q0);

	if(flag == 1) {
		if(temp == 0.0f) {
			return(0.0f);
		}
		temp = 1.0f/temp;
	}
	return(sign*temp);
}

// used by satan()
static float xatan(float arg){
	static float p4	= 0.161536412982230228262e2f;
	static float p3	= 0.26842548195503973794141e3f;
	static float p2	= 0.11530293515404850115428136e4f;
	static float p1	= 0.178040631643319697105464587e4f;
	static float p0	= 0.89678597403663861959987488e3f;
	static float q4	= 0.5895697050844462222791e2f;
	static float q3	= 0.536265374031215315104235e3f;
	static float q2	= 0.16667838148816337184521798e4f;
	static float q1	= 0.207933497444540981287275926e4f;
	static float q0	= 0.89678597403663861962481162e3f;

	float argsq;
	float value;

	argsq = arg*arg;
	value = ((((p4*argsq + p3)*argsq + p2)*argsq + p1)*argsq + p0);
	value = value/(((((argsq + q4)*argsq + q3)*argsq + q2)*argsq + q1)*argsq + q0);
	return(value*arg);
}

// used by atan() and atan2()
static float satan(float arg){
	static float sq2p1	= 2.414213562373095048802e0f;
	static float sq2m1	= 0.414213562373095048802e0f;

	if(arg < sq2m1)
		return(xatan(arg));
	else if(arg > sq2p1)
		return(PI_2 - xatan(1.0f/arg));
	else
		return(PI_4 + xatan((arg-1.0f)/(arg+1.0f)));
}

// atan
float atan(float arg){
	if(arg>0)
		return(satan(arg));
	else
		return(-satan(-arg));
}

// atan2
float atan2(float arg1, float arg2){
	if((arg1+arg2)==arg1)
		if(arg1 >= 0.0f) return(PI_2);
		else return(-PI_2);
	else if(arg2 < 0.0f)
		if(arg1 >= 0.0f)
			return(PI - satan(-arg1/arg2));
		else
			return(-PI + satan(arg1/arg2));
	else if(arg1 > 0.0f)
		return(satan(arg1/arg2));
	else
		return(-satan(-arg1/arg2));
}

// asin
float asin(float arg){
	float sign, temp;

	sign = 1.0f;
	if(arg < 0.0f){
		arg = -arg;
		sign = -1.0f;
	}

	if(arg > 1.0f){
		return(0.0f);
	}

	temp = sqrt(1.0f - arg*arg);
	if(arg > 0.7f)
		temp = PI_2 - atan(temp/arg);
	else
		temp = atan(arg/temp);

	return(sign*temp);
}

// acos
float acos(float arg){
	if((arg > 1.0f) || (arg < -1.0f)){
		return(0.0f);
	}

	return(PI_2 - asin(arg));
}

// sinh
float sinh(float arg){
	static float p0	= -0.6307673640497716991184787251e+6f;
	static float p1	= -0.8991272022039509355398013511e+5f;
	static float p2	= -0.2894211355989563807284660366e+4f;
	static float p3	= -0.2630563213397497062819489e+2f;
	static float q0	= -0.6307673640497716991212077277e+6f;
	static float q1	=  0.1521517378790019070696485176e+5f;
	static float q2	= -0.173678953558233699533450911e+3f;

	float temp, argsq, sign;

	sign = 1.0f;
	if(arg < 0.0f){
		arg = - arg;
		sign = -1.0f;
	}

	if(arg > 21.0f){
		temp = exp(arg)/2.0f;
		if (sign > 0)
			return(temp);
		else
			return(-temp);
	}

	if(arg > 0.5f){
		return(sign*(exp(arg) - exp(-arg))/2.0f);
	}

	argsq = arg*arg;
	temp = (((p3*argsq+p2)*argsq+p1)*argsq+p0)*arg;
	temp /= (((argsq+q2)*argsq+q1)*argsq+q0);
	return(sign*temp);
}

// cosh
float cosh(float arg){
	if(arg < 0.0f)
		arg = -arg;
	if(arg > 21.0f) {
		return(exp(arg)/2.0f);
	}

	return((exp(arg) + exp(-arg))/2.0f);
}

// tanh
float tanh(float arg){
	float sign;

	sign = 1.0f;
	if(arg < 0.0f){
		arg = -arg;
		sign = -1.0f;
	}

	if(arg > 21.0f)
		return(sign);

	return(sign*sinh(arg)/cosh(arg));
}

