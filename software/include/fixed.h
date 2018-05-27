typedef int32_t fixed_t;

#ifndef FIX_IBITS
#define FIX_IBITS		16	
#endif

#ifndef FIX_MULDIV_WIDTH
#define FIX_MULDIV_WIDTH	32
#endif

#define FIX_FBITS		(32 - FIX_IBITS)
#define FIX_FMASK		(((fixed_t)1 << FIX_FBITS) - 1)
#define FIX_ONE			((fixed_t)((fixed_t)1 << FIX_FBITS))

#define fix_val(V)		((fixed_t)((V) * FIX_ONE + ((V) >= 0 ? 0.5 : -0.5)))
#define fix_int(F)		((F) >> FIX_FBITS)
#define fix_frac(A)		((fixed_t)(A) & FIX_FMASK)
#define fix_abs(A)		((A) < 0 ? -(A) : (A))
#define fix_add(A,B)		((A) + (B))
#define fix_sub(A,B)		((A) - (B))

#define FIX_HALF		(FIX_ONE >> 1)
#define FIX_TWO			(FIX_ONE + FIX_ONE)
#define FIX_PI			fix_val(3.14159265358979323846)
#define FIX_TWO_PI		fix_val(6.28318530717958647692)
#define FIX_HALF_PI		fix_val(1.57079632679489661923)
#define FIX_FOURTH_PI		fix_val(0.78539816339744830961)
#define FIX_E			fix_val(2.71828182845904523536)
#define FIX_LN2			fix_val(0.69314718055994530942)
#define FIX_LN2_INV		fix_val(1.44269504088896340736)

#if FIX_MULDIV_WIDTH == 64
typedef	int64_t	fixedd_t;

#define fix_mul(A,B)		((fixed_t)(((fixedd_t)(A) * (fixedd_t)(B)) >> FIX_FBITS))
#define fix_div(A,B)		((fixed_t)(((fixedd_t)(A) << FIX_FBITS) / (fixedd_t)(B)))

#else

fixed_t fix_mul(fixed_t x, fixed_t y)
{
	uint32_t neg = 0, a, b, c, d;
	fixed_t res;

	if (x < 0){
		x = -x;
		neg = 1;
	}

	if (y < 0){
		y = -y;
		neg = neg ^ 1;
	}

	a = x >> FIX_FBITS;
	b = x & FIX_FMASK;
	c = y >> FIX_FBITS;
	d = y & FIX_FMASK;
	res = (fixed_t)(((d * b) >> FIX_FBITS) + (d * a) + (c * b) + ((c * a) << FIX_FBITS));

	return (neg ? -res : res);
}

fixed_t fix_div(fixed_t x, fixed_t y)
{
	uint32_t neg = 0;
	fixed_t res;

	if (x < 0){
		x = -x;
		neg = 1;
	}

	if (y < 0){
		y = -y;
		neg = neg ^ 1;
	}

#if FIX_IBITS <= 16
	res = fix_mul(x, ((0x80000000 / y)) << (33 - FIX_IBITS * 2));
#else
	res = fix_mul(x, ((0x80000000 / y)) >> (FIX_IBITS * 2 - 33));
#endif
	return (neg ? -res : res);
}
#endif

void fixtoa(fixed_t a, char *str, int32_t dec)
{
	int32_t ndec = 0, slen = 0;
	int8_t tmp[20];
	uint32_t fr, ip;

	if (a < 0){
		str[slen++] = '-';
		a = -a;
	}

	ip = fix_int(a);
	do {
		tmp[ndec++] = '0' + ip % 10;
		ip /= 10;
	} while (ip != 0);

	while (ndec > 0)
		str[slen++] = tmp[--ndec];
	str[slen++] = '.';

	fr = fix_frac(a);
	do {
		fr = (fr & FIX_FMASK) * 10;

		str[slen++] = '0' + (fr >> FIX_FBITS) % 10;
		ndec++;
	} while (ndec < dec);
	str[slen] = '\0';
}

fixed_t fix_sqrt(fixed_t a)
{
	int32_t inv = 0, l, i, s, itr = FIX_FBITS;

	if (a < 0)
		return -1;
	if (a == 0 || a == FIX_ONE)
		return a;
		
	if (a < FIX_ONE && a > 6){
		inv = 1;
		a = fix_div(FIX_ONE, a);
	}
	
	if (a > FIX_ONE){
		s = a;
		itr = 0;
		while (s > 0){
			s >>= 2;
			itr++;
		}
	}

	l = (a >> 1) + 1;
	for (i = 0; i < itr; i++)
		l = (l + fix_div(a, l)) >> 1;
		
	if (inv)
		return fix_div(FIX_ONE, l);
		
	return l;
}

fixed_t fix_exp(fixed_t fp)
{
	fixed_t xabs, k, z, r, xp;
	fixed_t p0 = fix_val( 1.66666666666666019037e-01);
	fixed_t p1 = fix_val(-2.77777777770155933842e-03);
	fixed_t p2 = fix_val( 6.61375632143793436117e-05);
	fixed_t p3 = fix_val(-1.65339022054652515390e-06);
	fixed_t p4 = fix_val( 4.13813679705723846039e-08);

	if (fp == 0)
		return FIX_ONE;

	xabs = fix_abs(fp);
	k = fix_mul(xabs, FIX_LN2_INV);
	k += FIX_HALF;
	k &= ~FIX_FMASK;
	
	if (fp < 0)
		k = -k;

	fp -= fix_mul(k, FIX_LN2);
	z = fix_mul(fp, fp);

	r = FIX_TWO + fix_mul(z, p0 + fix_mul(z, p1 + fix_mul(z, p2 + fix_mul(z, p3 + fix_mul(z, p4)))));
	xp = FIX_ONE + fix_div(fix_mul(fp, FIX_TWO), r - fp);
	
	if (k < 0)
		k = FIX_ONE >> (-k >> FIX_FBITS);
	else
		k = FIX_ONE << (k >> FIX_FBITS);
		
	return fix_mul(k, xp);
}

fixed_t fix_ln(fixed_t fp)
{
	fixed_t log2, xi;
	fixed_t f, s, z, w, r;
	fixed_t p0 = fix_val(6.666666666666735130e-01);
	fixed_t p1 = fix_val(3.999999999940941908e-01);
	fixed_t p2 = fix_val(2.857142874366239149e-01);
	fixed_t p3 = fix_val(2.222219843214978396e-01);
	fixed_t	p4 = fix_val(1.818357216161805012e-01);
	fixed_t p5 = fix_val(1.531383769920937332e-01);
	fixed_t p6 = fix_val(1.479819860511658591e-01);

	if (fp < 0)
		return 0;
	if (fp == 0)
		return -1;

	log2 = 0;
	xi = fp;
	while (xi > FIX_TWO) {
		xi >>= 1;
		log2++;
	}
	f = xi - FIX_ONE;
	s = fix_div(f, FIX_TWO + f);
	z = fix_mul(s, s);
	w = fix_mul(z, z);
	r = fix_mul(w, p1 + fix_mul(w, p3 + fix_mul(w, p5))) + fix_mul(z, p0 + fix_mul(w, p2 + fix_mul(w, p4 + fix_mul(w, p6))));
	r = fix_mul(FIX_LN2, (log2 << FIX_FBITS)) + f - fix_mul(s, f - r);
	
	return r;
}

fixed_t fix_log(fixed_t fp, fixed_t base)
{
	return fix_div(fix_ln(fp), fix_ln(base));
}

fixed_t fix_pow(fixed_t fp, fixed_t exp)
{
	if (exp == 0)
		return FIX_ONE;
		
	if (fp < 0)
		return 0;
		
	return fix_exp(fix_mul(fix_ln(fp), exp));
}

fixed_t fix_rad(fixed_t deg)
{
	return fix_div(fix_mul(deg, FIX_PI), fix_val(180.0));
}

fixed_t fix_sin(fixed_t rad)
{
	fixed_t sine;

	while (rad < -FIX_PI)
		rad += FIX_TWO_PI;
	while (rad > FIX_PI)
		rad -= FIX_TWO_PI;

	if (rad < 0)
		sine = fix_mul(fix_val(1.27323954), rad) + fix_mul(fix_mul(fix_val(0.405284735), rad), rad);
	else
		sine = fix_mul(fix_val(1.27323954), rad) - fix_mul(fix_mul(fix_val(0.405284735), rad), rad);

	if (sine < 0)
		sine = fix_mul(fix_val(0.225), (fix_mul(sine, -sine) - sine)) + sine;
	else
		sine = fix_mul(fix_val(0.225), (fix_mul(sine, sine) - sine)) + sine;

	return sine;
}

fixed_t fix_cos(fixed_t rad)
{
	return fix_sin((rad + FIX_HALF_PI));
}

fixed_t fix_tan(fixed_t rad)
{
	return fix_div(fix_sin(rad), fix_cos(rad));
}

static fixed_t xatan(fixed_t arg)
{
	fixed_t p4 = fix_val(0.161536412982230228262e2);
	fixed_t p3 = fix_val(0.26842548195503973794141e3);
	fixed_t p2 = fix_val(0.11530293515404850115428136e4);
	fixed_t p1 = fix_val(0.178040631643319697105464587e4);
	fixed_t p0 = fix_val(0.89678597403663861959987488e3);
	fixed_t q4 = fix_val(0.5895697050844462222791e2);
	fixed_t q3 = fix_val(0.536265374031215315104235e3);
	fixed_t q2 = fix_val(0.16667838148816337184521798e4);
	fixed_t q1 = fix_val(0.207933497444540981287275926e4);
	fixed_t q0 = fix_val(0.89678597403663861962481162e3);
	fixed_t argsq, value;

	argsq = fix_mul(arg, arg);
	value = fix_mul((fix_mul((fix_mul((fix_mul(p4, argsq) + p3), argsq) + p2), argsq) + p1), argsq) + p0;
	value = fix_div(value, (fix_mul((fix_mul((fix_mul((fix_mul((argsq + q4), argsq) + q3), argsq) + q2), argsq) + q1), argsq) + q0));
	
	return fix_mul(value, arg);
}

static fixed_t satan(fixed_t arg)
{
	fixed_t sq2p1 = fix_val(2.414213562373095048802e0);
	fixed_t sq2m1 = fix_val(0.414213562373095048802e0);

	if (arg < sq2m1)
		return xatan(arg);
	else if (arg > sq2p1)
		return (FIX_HALF_PI - xatan(fix_div(fix_val(1.0), arg)));
	else
		return (FIX_FOURTH_PI + xatan(fix_div((arg - fix_val(1.0)), (arg + fix_val(1.0)))));
}

fixed_t fix_atan(fixed_t arg)
{
	if (arg > 0)
		return satan(arg);
	else
		return -satan(-arg);
}

fixed_t fix_atan2(fixed_t arg1, fixed_t arg2)
{
	if ((arg1 + arg2) == arg1)
		if(arg1 >= fix_val(0.0))
			return FIX_HALF_PI;
		else
			return -FIX_HALF_PI;
	else if (arg2 < fix_val(0.0))
		if(arg1 >= fix_val(0.0))
			return (FIX_PI - satan(fix_div(-arg1, arg2)));
		else
			return (-FIX_PI + satan(fix_div(arg1, arg2)));
	else if (arg1 > fix_val(0.0))
		return satan(fix_div(arg1, arg2));
	else
		return -satan(fix_div(-arg1, arg2));
}

fixed_t fix_asin(fixed_t arg)
{
	fixed_t sign, temp;

	sign = fix_val(1.0);
	if (arg < fix_val(0.0)){
		arg = -arg;
		sign = fix_val(-1.0);
	}

	if(arg > fix_val(1.0))
		return fix_val(0.0);

	temp = fix_sqrt(fix_val(1.0) - fix_mul(arg, arg));
	if(arg > fix_val(0.7))
		temp = FIX_HALF_PI - fix_atan(fix_div(temp, arg));
	else
		temp = fix_atan(fix_div(arg, temp));

	return fix_mul(sign, temp);
}

fixed_t fix_acos(fixed_t arg)
{
	if ((arg > fix_val(1.0)) || (arg < fix_val(-1.0))){
		return fix_val(0.0);
	}

	return (FIX_HALF_PI - fix_asin(arg));
}

fixed_t fix_sinh(fixed_t arg)
{
	fixed_t temp, sign;

	sign = fix_val(1.0);
	if (arg < fix_val(0.0)){
		arg = - arg;
		sign = fix_val(-1.0);
	}

	if (arg > fix_val(21.0)){
		temp = fix_exp(arg) >> 1;
		if (sign > 0)
			return temp;
		else
			return -temp;
	}

	temp = fix_mul(sign, (fix_exp(arg) - fix_exp(-arg))) >> 1;
	
	return temp;
}

fixed_t fix_cosh(fixed_t arg)
{
	fixed_t temp;

	if (arg < fix_val(0.0))
		arg = -arg;

	if (arg > fix_val(21.0)){
		return fix_exp(arg) >> 1;
	}

	temp = (fix_exp(arg) + fix_exp(-arg)) >> 1;

	return temp;
}

fixed_t fix_tanh(fixed_t arg)
{
	fixed_t sign, temp;

	sign = fix_val(1.0);
	if (arg < fix_val(0.0)){
		arg = -arg;
		sign = fix_val(-1.0);
	}

	if(arg > fix_val(21.0))
		return sign;

	temp = fix_div(fix_mul(sign, fix_sinh(arg)), fix_cosh(arg));

	return temp;
}
