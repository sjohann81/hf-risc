#include <hf-risc.h>
#include <fp-math.h>

#define FtoL(X)		(*(uint32_t *)&(X))
#define LtoF(X)		(*(float *)&(X))

/* add two floats */
float __addsf3(float a1, float a2){
	int32_t mant1, mant2;
	union float_long fl1, fl2;
	int32_t exp1, exp2;
	int32_t sign = 0;

	fl1.f = a1;
	fl2.f = a2;

	/* check for zero args */
	if (!fl1.l){
		fl1.f = fl2.f;
		goto test_done;
	}
	if (!fl2.l)
		goto test_done;

	exp1 = EXP(fl1.l);
	exp2 = EXP(fl2.l);

	if (exp1 > exp2 + 25)
		goto test_done;
	if (exp2 > exp1 + 25){
		fl1.f = fl2.f;
		goto test_done;
	}

	/* do everything in excess precision so's we can round later */
	mant1 = MANT (fl1.l) << 6;
	mant2 = MANT (fl2.l) << 6;

	if (SIGN (fl1.l))
		mant1 = -mant1;
	if (SIGN (fl2.l))
		mant2 = -mant2;

	if (exp1 > exp2){
		mant2 >>= exp1 - exp2;
	}else{
		mant1 >>= exp2 - exp1;
		exp1 = exp2;
	}
	mant1 += mant2;

	if (mant1 < 0){
		mant1 = -mant1;
		sign = SIGNBIT;
	}else{
		if (!mant1){
			fl1.f = 0;
			goto test_done;
  		}
	}

	/* normalize up */
	while (!(mant1 & 0xE0000000)){
		mant1 <<= 1;
		exp1--;
	}

	/* normalize down? */
	if (mant1 & (1 << 30)){
		mant1 >>= 1;
		exp1++;
	}

	/* round to even */
	mant1 += (mant1 & 0x40) ? 0x20 : 0x1F;

	/* normalize down? */
	if (mant1 & (1 << 30)){
		mant1 >>= 1;
		exp1++;
	}

	/* lose extra precision */
	mant1 >>= 6;

	/* turn off hidden bit */
	mant1 &= ~HIDDEN;

	/* pack up and go home */
	fl1.l = PACK (sign, exp1, mant1);
test_done:
	return (fl1.f);
}

/* subtract two floats */
float __subsf3(float a1, float a2){
	union float_long fl1, fl2;

	fl1.f = a1;
	fl2.f = a2;

	/* check for zero args */
	if (!fl2.l)
		return (fl1.f);
	if (!fl1.l)
		return (-fl2.f);

	/* twiddle sign bit and add */
	fl2.l ^= SIGNBIT;
	return __addsf3(a1, fl2.f);
}

/* compare two floats */
int32_t __cmpsf2(float a1, float a2){
	union float_long fl1, fl2;
	int32_t i = 0;

	fl1.f = a1;
	fl2.f = a2;

	if (SIGN (fl1.l) && SIGN (fl2.l)){
		fl1.l ^= SIGNBIT;
		fl2.l ^= SIGNBIT;
		i = 1;
	}
	if (i == 0){
		if (fl1.l < fl2.l)
			return (-1);
		if (fl1.l > fl2.l)
			return (1);
		return (0);
	}else{
		if (fl1.l > fl2.l)
			return (-1);
		if (fl1.l < fl2.l)
			return (1);
		return (0);
	}
}

int32_t __ltsf2(float a, float b){
	return -(__cmpsf2 (a, b) < 0);
}

int32_t __lesf2(float a, float b){
	return __cmpsf2 (a, b) > 0;
}

int32_t __gtsf2(float a, float b){
	return __cmpsf2 (a, b) > 0;
}

int32_t __gesf2(float a, float b){
	return (__cmpsf2 (a, b) >= 0) - 1;
}

int32_t __eqsf2(float a, float b){
	return !(*(int32_t *) & a == *(int32_t *) & b);
}

int32_t __nesf2(float a, float b){
	return *(int32_t *) & a != *(int32_t *) & b;
}

/* multiply two floats */
float __mulsf3(float a1, float a2){
	union float_long fl1, fl2;
	uint32_t result;
	int32_t exp;
	int32_t sign;

	fl1.f = a1;
	fl2.f = a2;

	if (!fl1.l || !fl2.l){
		fl1.f = 0;
		goto test_done;
	}

	/* compute sign and exponent */
	sign = SIGN (fl1.l) ^ SIGN (fl2.l);
	exp = EXP(fl1.l) - EXCESS;
	exp += EXP(fl2.l);

	fl1.l = MANT (fl1.l);
	fl2.l = MANT (fl2.l);

	/* the multiply is done as one 16x16 multiply and two 16x8 multiplies */
	result = (fl1.l >> 8) * (fl2.l >> 8);
	result += ((fl1.l & 0xFF) * (fl2.l >> 8)) >> 8;
	result += ((fl2.l & 0xFF) * (fl1.l >> 8)) >> 8;

	result >>= 2;
	if (result & 0x20000000){
		/* round */
		result += 0x20;
		result >>= 6;
	}else{
		/* round */
		result += 0x10;
		result >>= 5;
		exp--;
	}
	if (result & (HIDDEN<<1)){
		result >>= 1;
		exp++;
	}

	result &= ~HIDDEN;

	/* pack up and go home */
	fl1.l = PACK(sign, exp, result);
test_done:
	return (fl1.f);
}

/* divide two floats */
float __divsf3(float a1, float a2){
	union float_long fl1, fl2;
	int32_t result;
	int32_t mask;
	int32_t exp, sign;

	fl1.f = a1;
	fl2.f = a2;

	/* subtract exponents */
	exp = EXP(fl1.l) - EXP(fl2.l) + EXCESS;

	/* compute sign */
	sign = SIGN(fl1.l) ^ SIGN(fl2.l);

	/* divide by zero??? */
	if (!fl2.l)
		/* return NaN or -NaN */
		return (sign ? 0xFFFFFFFF : 0x7FFFFFFF);

	/* numerator zero??? */
	if (!fl1.l)
		return (0);

	/* now get mantissas */
	fl1.l = MANT (fl1.l);
	fl2.l = MANT (fl2.l);

	/* this assures we have 25 bits of precision in the end */
	if (fl1.l < fl2.l){
		fl1.l <<= 1;
		exp--;
	}

	/* now we perform repeated subtraction of fl2.l from fl1.l */
	mask = 0x1000000;
	result = 0;
	while (mask){
		if (fl1.l >= fl2.l){
			result |= mask;
			fl1.l -= fl2.l;
		}
		fl1.l <<= 1;
		mask >>= 1;
	}

	/* round */
	result += 1;

	/* normalize down */
	exp++;
	result >>= 1;

	result &= ~HIDDEN;

	/* pack up and go home */
	fl1.l = PACK(sign, exp, result);
	return (fl1.f);
}

/* negate a float */
float __negsf2(float a1){
	union float_long fl1;

	fl1.f = a1;
	if (!fl1.l)
		return (0);

	fl1.l ^= SIGNBIT;
	return (fl1.f);
}

/* convert float to int32_t */
int32_t __fixsfsi(float a_fp){
	uint32_t a;
	uint32_t as;
	int32_t ae;
	int32_t af, shift;

	a = FtoL(a_fp);
	as = a >> 31;
	ae = (a >> 23) & 0xff;
	af = 0x00800000 | (a & 0x007fffff);
	af <<= 7;
	shift = -(ae - 0x80 - 29);
	if(shift > 0){
		if(shift < 31) 
			af >>= shift;
		else 
			af = 0;
	}
	af = as ? -af: af;

	return af;
}

uint32_t __fixunssfsi(float a_fp){
	uint32_t a;
	uint32_t as;
	int32_t ae;
	int32_t af, shift;

	a = FtoL(a_fp);
	as = a >> 31;
	ae = (a >> 23) & 0xff;
	af = 0x00800000 | (a & 0x007fffff);
	af <<= 7;
	shift = -(ae - 0x80 - 29);
	if(shift > 0){
		if(shift < 31) 
			af >>= shift;
		else 
			af = 0;
	}
	af = as ? -af: af;

	return af;
}

/* convert int32_t to float */
float __floatsisf(int32_t af){
	uint32_t a;
	uint32_t as, ae;

	as = af>=0 ? 0: 1;
	af = af>=0 ? af: -af;
	ae = 0x80 + 22;
	if(af == 0) 
		return LtoF(af);
	while(af & 0xff000000){
		++ae;
		af >>= 1;
	}
	while((af & 0xff800000) == 0){
		--ae;
		af <<= 1;
	}
	a = (as << 31) | (ae << 23) | (af & 0x007fffff);

	return LtoF(a);
}

float __floatunsisf(uint32_t af){
	uint32_t a;
	uint32_t as, ae;

	as = af>=0 ? 0: 1;
	af = af>=0 ? af: -af;
	ae = 0x80 + 22;
	if(af == 0) 
		return LtoF(af);
	while(af & 0xff000000){
		++ae;
		af >>= 1;
	}
	while((af & 0xff800000) == 0){
		--ae;
		af <<= 1;
	}
	a = (as << 31) | (ae << 23) | (af & 0x007fffff);

	return LtoF(a);
}

