/* file:          libc.c
 * description:   small C library
 * date:          09/2015
 * author:        Sergio Johann Filho <sergio.filho@pucrs.br>
 */

#include <hf-risc.h>

/*
auxiliary routines
*/

void delay_ms(uint32_t msec)
{
	volatile uint32_t cur, last, delta, msecs;
	uint32_t cycles_per_msec;

	last = TIMER0;
	delta = msecs = 0;
	cycles_per_msec = CPU_SPEED / 1000;
	while (msec > msecs) {
		cur = TIMER0;
		if (cur < last)
			delta += (cur + (CPU_SPEED - last));
		else
			delta += (cur - last);
		last = cur;
		if (delta >= cycles_per_msec) {
			msecs += delta / cycles_per_msec;
			delta %= cycles_per_msec;
		}
	}
}

void delay_us(uint32_t usec)
{
	volatile uint32_t cur, last, delta, usecs;
	uint32_t cycles_per_usec;

	last = TIMER0;
	delta = usecs = 0;
	cycles_per_usec = CPU_SPEED / 1000000;
	while (usec > usecs) {
		cur = TIMER0;
		if (cur < last)
			delta += (cur + (CPU_SPEED - last));
		else
			delta += (cur - last);
		last = cur;
		if (delta >= cycles_per_usec) {
			usecs += delta / cycles_per_usec;
			delta %= cycles_per_usec;
		}
	}
}

/*
minimal custom C library
*/

#ifndef DEBUG_PORT
void putchar(int32_t value){		// polled putchar()
	while (UARTCAUSE & MASK_UART0_WRITEBUSY);
	UART0 = value;
}

int32_t kbhit(void){
	return UARTCAUSE & MASK_UART0_DATAAVAIL;
}

int32_t getchar(void){			// polled getch()
	while (!kbhit());
	return UART0;
}
#else
void putchar(int32_t value){		// polled putchar()
	DEBUG_ADDR = value;
}

int32_t kbhit(void){
	return 0;
}

int32_t getchar(void){			// polled getch()
	return DEBUG_ADDR;
}
#endif

int8_t *strcpy(int8_t *dst, const int8_t *src){
	int8_t *dstSave=dst;
	int32_t c;

	do{
		c = *dst++ = *src++;
	} while(c);
	return dstSave;
}

int8_t *strncpy(int8_t *s1, int8_t *s2, int32_t n){
	int32_t i;
	int8_t *os1;

	os1 = s1;
	for (i = 0; i < n; i++)
		if ((*s1++ = *s2++) == '\0') {
			while (++i < n)
				*s1++ = '\0';
			return(os1);
		}
	return(os1);
}

int8_t *strcat(int8_t *dst, const int8_t *src){
	int32_t c;
	int8_t *dstSave=dst;

	while(*dst)
		++dst;
	do{
		c = *dst++ = *src++;
	} while(c);

	return dstSave;
}

int8_t *strncat(int8_t *s1, int8_t *s2, int32_t n){
	int8_t *os1;

	os1 = s1;
	while (*s1++);
	--s1;
	while ((*s1++ = *s2++))
		if (--n < 0) {
			*--s1 = '\0';
			break;
		}
	return(os1);
}

int32_t strcmp(const int8_t *s1, const int8_t *s2){
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return(0);

	return(*s1 - *--s2);
}

int32_t strncmp(int8_t *s1, int8_t *s2, int32_t n){
	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);

	return(n<0 ? 0 : *s1 - *--s2);
}

int8_t *strstr(const int8_t *string, const int8_t *find){
	int32_t i;

	for(;;){
		for(i = 0; string[i] == find[i] && find[i]; ++i);
		if(find[i] == 0)
			return (char *)string;
		if(*string++ == 0)
			return NULL;
	}
}

int32_t strlen(const int8_t *s){
	int32_t n;

	n = 0;
	while (*s++)
		n++;

	return(n);
}

int8_t *strchr(const int8_t *s, int32_t c){
	while (*s != (int8_t)c)
		if (!*s++)
			return 0;

	return (int8_t *)s;
}

int8_t *strpbrk(int8_t *str, int8_t *set){
	int8_t c, *p;

	for (c = *str; c != 0; str++, c = *str) {
		for (p = set; *p != 0; p++) {
			if (c == *p) {
				return str;
			}
		}
	}
	return 0;

}

int8_t *strsep(int8_t **pp, int8_t *delim){
	int8_t *p, *q;

	if (!(p = *pp))
		return 0;
	if ((q = strpbrk (p, delim))){
		*pp = q + 1;
		*q = '\0';
	}else	*pp = 0;

	return p;
}

int8_t *strtok(int8_t *s, const int8_t *delim){
	const int8_t *spanp;
	int32_t c, sc;
	int8_t *tok;
	static int8_t *last;

	if (s == NULL && (s = last) == NULL)
		return (NULL);

	cont:
	c = *s++;
	for (spanp = delim; (sc = *spanp++) != 0;){
		if (c == sc)
		goto cont;
	}

	if (c == 0){
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	for(;;){
		c = *s++;
		spanp = delim;
		do{
			if ((sc = *spanp++) == c){
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		}while (sc != 0);
	}
}

void *memcpy(void *dst, const void *src, uint32_t n){
	int8_t *r1 = dst;
	const int8_t *r2 = src;

	while (n--)
		*r1++ = *r2++;

	return dst;
}

void *memmove(void *dst, const void *src, uint32_t n){
	int8_t *s = (int8_t *)dst;
	const int8_t *p = (const int8_t *)src;

	if (p >= s){
		while (n--)
			*s++ = *p++;
	}else{
		s += n;
		p += n;
		while (n--)
			*--s = *--p;
	}

	return dst;
}

int32_t memcmp(const void *cs, const void *ct, uint32_t n){
	const uint8_t *r1 = (const uint8_t *)cs;
	const uint8_t *r2 = (const uint8_t *)ct;

	while (n && (*r1 == *r2)) {
		++r1;
		++r2;
		--n;
	}

	return (n == 0) ? 0 : ((*r1 < *r2) ? -1 : 1);
}

void *memset(void *s, int32_t c, uint32_t n){
	uint8_t *p = (uint8_t *)s;

	while (n--)
		*p++ = (uint8_t)c;

	return s;
}

int32_t strtol(const int8_t *s, int8_t **end, int32_t base){
	int32_t i;
	uint32_t ch, value=0, neg=0;

	if(s[0] == '-'){
		neg = 1;
		++s;
	}
	if(s[0] == '0' && s[1] == 'x'){
		base = 16;
		s += 2;
	}
	for(i = 0; i <= 8; ++i){
		ch = *s++;
		if('0' <= ch && ch <= '9')
			ch -= '0';
		else if('A' <= ch && ch <= 'Z')
			ch = ch - 'A' + 10;
		else if('a' <= ch && ch <= 'z')
			ch = ch - 'a' + 10;
		else
			break;
		value = value * base + ch;
	}
	if(end)
		*end = (char*)s - 1;
	if(neg)
		value = -(int32_t)value;
	return value;
}

int32_t atoi(const int8_t *s){
	int32_t n, f;

	n = 0;
	f = 0;
	for(;;s++){
		switch(*s){
		case ' ':
		case '\t':
			continue;
		case '-':
			f++;
		case '+':
			s++;
		}
		break;
	}
	while(*s >= '0' && *s <= '9')
		n = n*10 + *s++ - '0';
	return(f?-n:n);
}

float atof(const int8_t *p){
	float val, power;
	int32_t i, sign;

	for (i = 0; isspace(p[i]); i++);

	sign = (p[i] == '-') ? -1 : 1;

	if (p[i] == '+' || p[i] == '-')
		i++;
	for (val = 0.0f; isdigit(p[i]); i++)
		val = 10.0f * val + (p[i] - '0');

	if (p[i] == '.')
		i++;
	for (power = 1.0f; isdigit(p[i]); i++){
		val = 10.0f * val + (p[i] - '0');
		power *= 10.0f;
	}

	return sign * val / power;
}

int32_t ftoa(float f, int8_t *outbuf, int32_t precision){
	int32_t mantissa, int_part, frac_part, exp2, i;
	int8_t *p;
	union float_long fl;

	p = outbuf;

	if (f < 0.0){
		*p = '-';
		f = -f;
		p++;
	}

	fl.f = f;

	exp2 = (fl.l >> 23) - 127;
	mantissa = (fl.l & 0xffffff) | 0x800000;
	frac_part = 0;
	int_part = 0;

	if (exp2 >= 31){
		return -1;	/* too large */
	}else{
		if (exp2 < -23){
//			return -1;	/* too small */
		}else{
			if (exp2 >= 23){
				int_part = mantissa << (exp2 - 23);
			}else{
				if (exp2 >= 0){
					int_part = mantissa >> (23 - exp2);
					frac_part = (mantissa << (exp2 + 1)) & 0xffffff;
				}else{
					frac_part = (mantissa & 0xffffff) >> (-(exp2 + 1));
				}
			}
		}
	}

	if (int_part == 0){
		*p = '0';
		p++;
	}else{
		itoa(int_part, p, 10);
		while(*p) p++;
	}
	*p = '.';
	p++;

	for (i = 0; i < precision; i++){
		frac_part = (frac_part << 3) + (frac_part << 1);
		*p = (frac_part >> 24) + '0';
		p++;
		frac_part = frac_part & 0xffffff;
	}

	*p = 0;

	return 0;
}

int8_t *itoa(int32_t i, int8_t *s, int32_t base)
{
	int8_t c;
	int8_t *p = s;
	int8_t *q = s;
	uint32_t h;

	if (base == 16) {
		h = (uint32_t)i;
		do {
			*q++ = '0' + (h % base);
		} while (h /= base);
		if ((i >= 0) && (i < 16)) *q++ = '0';
		for (*q = 0; p <= --q; p++){
			(*p > '9') ? (c = *p + 39) : (c = *p);
			(*q > '9') ? (*p = *q + 39) : (*p = *q);
			*q = c;
		}
	} else {
		if (i >= 0) {
			do {
				*q++ = '0' + (i % base);
			} while (i /= base);
		} else {
			*q++ = '-';
			p++;
			do {
				*q++ = '0' - (i % base);
			} while (i /= base);
		}
		for (*q = 0; p <= --q; p++) {
			c = *p;
			*p = *q;
			*q = c;
		}
	}
	return s;
}

int32_t puts(const int8_t *str){
	while(*str)
		putchar(*str++);
	putchar('\n');

	return 0;
}

int8_t *gets(int8_t *s){
	int32_t c;
	int8_t *cs;

	cs = s;
	while ((c = getchar()) != '\n' && c >= 0)
		*cs++ = c;
	if (c<0 && cs==s)
		return(NULL);
	*cs++ = '\0';
	return(s);
}

int8_t *getline(int8_t *s){
	int32_t c, i = 0;
	int8_t *cs;

	cs = s;
	while ((c = getchar()) != '\n' && c >= 0) {
		if (++i == 80) {
			*cs = '\0';
			break;
		}
		*cs++ = c;
	}
	if (c<0 && cs==s)
		return(NULL);
	*cs++ = '\0';
	return(s);
}

int32_t abs(int32_t n){
	return n >= 0 ? n : -n;
}

static uint32_t rand1=0xbaadf00d;

int32_t random(void){
	rand1 = rand1 * 1103515245 + 12345;
	return (uint32_t)(rand1 >> 16) & 32767;
}

void srand(uint32_t seed){
	rand1 = seed;
}

int32_t hexdump(int8_t *buf, uint32_t size){
	uint32_t k, l;
	int8_t ch;

	for(k = 0; k < size; k += 16){
		printf("\n%08x ", buf + k);
		for(l = 0; l < 16; l++){
			printf("%02x ", buf[k + l]);
			if (l == 7) putchar(' ');
		}
		printf(" |");
		for(l = 0; l < 16; l++){
			ch = buf[k + l];
			if ((ch >= 32) && (ch <= 126))
				putchar(ch);
			else
				putchar('.');
		}
		putchar('|');
	}

	return 0;
}

/* printf() / sprintf() stuff */

static uint32_t divide(long *n, int base)
{
	uint32_t res;

	res = ((uint32_t)*n) % base;
	*n = (long)(((uint32_t)*n) / base);
	return res;
}

static int toint(const char **s)
{
	int i = 0;
	while (isdigit((int)**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

static void printchar(int8_t **str, int32_t c){
	if (str){
		**str = c;
		++(*str);
	}else{
		if (c) putchar(c);
	}
}

static int vsprintf(char **buf, const char *fmt, va_list args)
{
	char **p, *str;
	const char *digits = "0123456789abcdef";
	char pad, tmp[16];
	int width, base, sign, i;
	long num;

	for (p = buf; *fmt; fmt++) {
		if (*fmt != '%') {
			printchar(p, *fmt);
			continue;
		}
		/* get flags */
		++fmt;
		pad = ' ';
		if (*fmt == '0') {
			pad = '0';
			fmt++;
		}
		/* get width */
		width = -1;
		if (isdigit(*fmt)) {
			width = toint(&fmt);
		}
		/* ignore long */
		if (*fmt == 'l')
			fmt++;
		base = 10;
		sign = 0;
		switch (*fmt) {
		case 'c':
			printchar(p, (char)va_arg(args, int));
			continue;
		case 's':
			str = va_arg(args, char *);
			if (str == NULL)
				str = "<NULL>";
			for (; *str && width != 0; str++, width--) {
				printchar(p, *str);
			}
			while (width-- > 0)
				printchar(p, pad);
			continue;
		case 'X':
		case 'x':
			base = 16;
			break;
		case 'd':
			sign = 1;
			break;
		case 'u':
			break;
		default:
			continue;
		}
		num = va_arg(args, long);
		if (sign && num < 0) {
			num = -num;
			printchar(p, '-');
			width--;
		}
		i = 0;
		if (num == 0)
			tmp[i++] = '0';
		else
			while (num != 0)
				tmp[i++] = digits[divide(&num, base)];
		width -= i;
		while (width-- > 0)
			printchar(p, pad);
		while (i-- > 0)
			printchar(p, tmp[i]);
	}
	printchar(p, '\0');

	return 0;
}

int32_t printf(const int8_t *fmt, ...){
	va_list args;
	int32_t v;

	va_start(args, fmt);
	v = vsprintf(0, fmt, args);
	va_end(args);
	return v;
}

int32_t sprintf(int8_t *out, const int8_t *fmt, ...){
	va_list args;
	int32_t v;

	va_start(args, fmt);
	v = vsprintf(&out, fmt, args);
	va_end(args);
	return v;
}


/*
software implementation of multiply/divide and 64-bit routines
*/

typedef union{
	int64_t all;
	struct{
#if LITTLE_ENDIAN
		uint32_t low;
		int32_t high;
#else
		int32_t high;
		uint32_t low;
#endif
	} s;
} dwords;

int32_t __mulsi3(uint32_t a, uint32_t b){
	uint32_t answer = 0;

	while(b){
		if(b & 1)
			answer += a;
		a <<= 1;
		b >>= 1;
	}
	return answer;
}

int64_t __muldsi3(uint32_t a, uint32_t b){
	dwords r;

	const int32_t bits_in_word_2 = (int32_t)(sizeof(int32_t) * 8) / 2;
	const uint32_t lower_mask = (uint32_t)~0 >> bits_in_word_2;
	r.s.low = (a & lower_mask) * (b & lower_mask);
	uint32_t t = r.s.low >> bits_in_word_2;
	r.s.low &= lower_mask;
	t += (a >> bits_in_word_2) * (b & lower_mask);
	r.s.low += (t & lower_mask) << bits_in_word_2;
	r.s.high = t >> bits_in_word_2;
	t = r.s.low >> bits_in_word_2;
	r.s.low &= lower_mask;
	t += (b >> bits_in_word_2) * (a & lower_mask);
	r.s.low += (t & lower_mask) << bits_in_word_2;
	r.s.high += t >> bits_in_word_2;
	r.s.high += (a >> bits_in_word_2) * (b >> bits_in_word_2);

	return r.all;
}

int64_t __muldi3(int64_t a, int64_t b){
	dwords x;
	x.all = a;
	dwords y;
	y.all = b;
	dwords r;
	r.all = __muldsi3(x.s.low, y.s.low);
/*	r.s.high += x.s.high * y.s.low + x.s.low * y.s.high; */
	r.s.high += __mulsi3(x.s.high, y.s.low) + __mulsi3(x.s.low, y.s.high);

	return r.all;
}

uint32_t __udivmodsi4(uint32_t num, uint32_t den, int32_t modwanted){
	uint32_t bit = 1;
	uint32_t res = 0;

	while (den < num && bit && !(den & (1L << 31))) {
		den <<= 1;
		bit <<= 1;
	}
	while (bit){
		if (num >= den){
			num -= den;
			res |= bit;
		}
		bit >>= 1;
		den >>= 1;
	}
	if (modwanted)
		return num;
	return res;
}

int32_t __divsi3(int32_t a, int32_t b){
	int32_t neg = 0;
	int32_t res;

	if (a < 0){
		a = -a;
		neg = !neg;
	}

	if (b < 0){
		b = -b;
		neg = !neg;
	}

	res = __udivmodsi4(a, b, 0);

	if (neg)
		res = -res;

	return res;
}

int32_t __modsi3(int32_t a, int32_t b){
	int32_t neg = 0;
	int32_t res;

	if (a < 0){
		a = -a;
		neg = 1;
	}

	if (b < 0)
		b = -b;

	res = __udivmodsi4(a, b, 1);

	if (neg)
		res = -res;

	return res;
}

uint32_t __udivsi3 (uint32_t a, uint32_t b){
	return __udivmodsi4(a, b, 0);
}

uint32_t __umodsi3 (uint32_t a, uint32_t b){
	return __udivmodsi4(a, b, 1);
}

int64_t __ashldi3(int64_t u, uint32_t b){
	dwords uu, w;
	uint32_t bm;

	if (b == 0)
		return u;

	uu.all = u;
	bm = 32 - b;

	if (bm <= 0){
		w.s.low = 0;
		w.s.high = (uint32_t) uu.s.low << -bm;
	}else{
		const uint32_t carries = (uint32_t) uu.s.low >> bm;

		w.s.low = (uint32_t) uu.s.low << b;
		w.s.high = ((uint32_t) uu.s.high << b) | carries;
	}

	return w.all;
}

int64_t __ashrdi3(int64_t u, uint32_t b){
	dwords uu, w;
	uint32_t bm;

	if (b == 0)
		return u;

	uu.all = u;
	bm = 32 - b;

	if (bm <= 0){
		/* w.s.high = 1..1 or 0..0 */
		w.s.high = uu.s.high >> 31;
		w.s.low = uu.s.low >> -bm;
	}else{
		const uint32_t carries = (uint32_t) uu.s.high << bm;

		w.s.high = uu.s.high >> b;
		w.s.low = ((uint32_t) uu.s.low >> b) | carries;
	}

	return w.all;
}

int64_t __lshrdi3(int64_t u, uint32_t b){
	dwords uu, w;
	uint32_t bm;

	if (b == 0)
		return u;

	uu.all = u;
	bm = 32 - b;

	if (bm <= 0){
		w.s.high = 0;
		w.s.low = (uint32_t) uu.s.high >> -bm;
	}else{
		const uint32_t carries = (uint32_t) uu.s.high << bm;

		w.s.high = (uint32_t) uu.s.high >> b;
		w.s.low = ((uint32_t) uu.s.low >> b) | carries;
	}

	return w.all;
}

uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem_p){
	uint64_t quot = 0, qbit = 1;

	if (den == 0){
//		return 1 / ((uint32_t)den);
		return 1;
	}

	while ((int64_t)den >= 0){
		den <<= 1;
		qbit <<= 1;
	}

	while (qbit){
		if (den <= num){
			num -= den;
			quot += qbit;
		}
		den >>= 1;
		qbit >>= 1;
	}

	if (rem_p)
		*rem_p = num;

	return quot;
}

uint64_t __umoddi3(uint64_t num, uint64_t den){
	uint64_t v = 0;

	(void) __udivmoddi4(num, den, &v);
	return v;
}

uint64_t __udivdi3(uint64_t num, uint64_t den){
	return __udivmoddi4(num, den, NULL);
}

int64_t __moddi3(int64_t num, int64_t den){
	int minus = 0;
	int64_t v = 0;

	if (num < 0){
		num = -num;
		minus = 1;
	}
	if (den < 0){
		den = -den;
		minus ^= 1;
	}

	(void) __udivmoddi4(num, den, (uint64_t *)&v);
	if (minus)
		v = -v;

	return v;
}

int64_t __divdi3(int64_t num, int64_t den){
	int minus = 0;
	int64_t v;

	if (num < 0){
		num = -num;
		minus = 1;
	}
	if (den < 0){
		den = -den;
		minus ^= 1;
	}

	v = __udivmoddi4(num, den, NULL);
	if (minus)
		v = -v;

	return v;
}

/*
software implementation of IEEE single-precision floating point
*/

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
	return -(__cmpsf2(a, b) < 0);
}

int32_t __lesf2(float a, float b){
	return __cmpsf2(a, b) > 0;
}

int32_t __gtsf2(float a, float b){
	return __cmpsf2(a, b) > 0;
}

int32_t __gesf2(float a, float b){
	return (__cmpsf2(a, b) >= 0) - 1;
}

int32_t __eqsf2(float a, float b){
	union float_long f1, f2;

	f1.f = a;
	f2.f = b;

	return !(f1.l == f2.l);
}

int32_t __nesf2(float a, float b){
	union float_long f1, f2;

	f1.f = a;
	f2.f = b;

	return (f1.l != f2.l);
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
	union float_long fb;

	fb.f = a_fp;
	a = fb.u;
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
	union float_long fb;

	fb.f = a_fp;
	a = fb.u;
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
	union float_long fb;

	as = af>=0 ? 0: 1;
	af = af>=0 ? af: -af;
	ae = 0x80 + 22;
	fb.l = af;
	if(af == 0)
		return fb.f;
	while(af & 0xff000000){
		++ae;
		af >>= 1;
	}
	while((af & 0xff800000) == 0){
		--ae;
		af <<= 1;
	}
	a = (as << 31) | (ae << 23) | (af & 0x007fffff);

	fb.u = a;

	return fb.f;
}

float __floatunsisf(uint32_t af){
	uint32_t a;
	uint32_t as, ae;
	union float_long fb;

	as = af>=0 ? 0: 1;
	af = af>=0 ? af: -af;
	ae = 0x80 + 22;
	fb.u = af;
	if(af == 0)
		return fb.f;
	while(af & 0xff000000){
		++ae;
		af >>= 1;
	}
	while((af & 0xff800000) == 0){
		--ae;
		af <<= 1;
	}
	a = (as << 31) | (ae << 23) | (af & 0x007fffff);

	fb.u = a;

	return fb.f;
}


/*
software implementation of IEEE double-precision floating point
*/

/* convert int to double */
double __floatsidf (long a1){
	long sign = 0, exp = 31 + EXCESSD;
	union double_long dl;

	if (!a1) {
		dl.l.upper = dl.l.lower = 0;
		return dl.d;
	}

	if (a1 < 0) {
		sign = SIGNBIT;
		a1 = -a1;
		if (a1 < 0) {
			dl.l.upper = SIGNBIT | ((32L + EXCESSD) << 20L);
			dl.l.lower = 0;
			return dl.d;
		}
	}

	while (a1 < 0x1000000) {
		a1 <<= 4;
		exp -= 4;
	}

	while (a1 < 0x40000000) {
		a1 <<= 1;
		exp--;
	}

	/* pack up and go home */
	dl.l.upper = sign;
	dl.l.upper |= exp << 20L;
	dl.l.upper |= (a1 >> 10L) & ~HIDDEND;
	dl.l.lower = a1 << 22L;

	return dl.d;
}

double __floatdidf (long long a1){
	int exp = 63 + EXCESSD;
	union double_long dl;

	dl.l.upper = dl.l.lower = 0;
	if (a1 == 0)
		return (dl.d);

	if (a1 < 0) {
		dl.l.upper = SIGNBIT;
		a1 = -a1;
	}

	while (a1 < (long long)1<<54) {
		a1 <<= 8;
		exp -= 8;
	}
	while (a1 < (long long)1<<62) {
		a1 <<= 1;
		exp -= 1;
	}

	/* pack up and go home */
	dl.ll |= (a1 >> 10) & ~HIDDEND_LL;
	dl.l.upper |= exp << 20;

	return (dl.d);
}

/* negate a double */
double __negdf2 (double a1){
	union double_long dl1;

	dl1.d = a1;

	if (!dl1.l.upper && !dl1.l.lower)
		return (dl1.d);

	dl1.l.upper ^= SIGNBIT;

	return (dl1.d);
}

/* convert float to double */
double __extendsfdf2 (float a1){
	union float_long fl1;
	union double_long dl;
	long exp;

	fl1.f = a1;

	if (!fl1.l) {
		dl.l.upper = dl.l.lower = 0;
		return dl.d;
	}

	dl.l.upper = SIGN (fl1.l);
	exp = EXP (fl1.l) - EXCESS + EXCESSD;
	dl.l.upper |= exp << 20;
	dl.l.upper |= (MANT (fl1.l) & ~HIDDEN) >> 3;
	dl.l.lower = MANT (fl1.l) << 29;

	return dl.d;
}

/* convert double to float */
float __truncdfsf2 (double a1){
	long exp;
	long mant;
	union float_long fl;
	union double_long dl1;

	dl1.d = a1;

	if (!dl1.l.upper && !dl1.l.lower)
		return 0;

	exp = EXPD (dl1) - EXCESSD + EXCESS;

	/* shift double mantissa 6 bits so we can round */
	mant = MANTD (dl1) >> 6;

	/* now round and shift down */
	mant += 1;
	mant >>= 1;

	/* did the round overflow? */
	if (mant & 0xFF000000) {
		mant >>= 1;
		exp++;
	}

	mant &= ~HIDDEN;

	/* pack up and go home */
	fl.l = PACK (SIGND (dl1), exp, mant);

	return (fl.f);
}


/* compare two doubles */
long __cmpdf2 (double a1, double a2){
	union double_long dl1, dl2;

	dl1.d = a1;
	dl2.d = a2;

	if (SIGND (dl1) && SIGND (dl2)) {
		dl1.l.upper ^= SIGNBIT;
		dl2.l.upper ^= SIGNBIT;
	}
	if (dl1.l.upper < dl2.l.upper)
		return (-1);
	if (dl1.l.upper > dl2.l.upper)
		return (1);
	if (dl1.l.lower < dl2.l.lower)
		return (-1);
	if (dl1.l.lower > dl2.l.lower)
		return (1);
	return (0);
}

/* convert double to int */
long __fixdfsi (double a1){
	union double_long dl1;
	int exp;
	long l;

	dl1.d = a1;

	if (!dl1.l.upper && !dl1.l.lower)
		return (0);

	exp = EXPD (dl1) - EXCESSD - 31;
	l = MANTD (dl1);

	if (exp > 0)
		return SIGND(dl1) ? (1<<31) : ((1ul<<31)-1);

	/* shift down until exp = 0 or l = 0 */
	if (exp < 0 && exp > -32 && l)
		l >>= -exp;
	else
		return (0);

	return (SIGND (dl1) ? -l : l);
}

/* convert double to int */
long long __fixdfdi (double a1){
	union double_long dl1;
	int exp;
	long long l;

	dl1.d = a1;

	if (!dl1.l.upper && !dl1.l.lower)
		return (0);

	exp = EXPD (dl1) - EXCESSD - 64;
	l = MANTD_LL(dl1);

	if (exp > 0) {
		l = (long long)1<<63;
		if (!SIGND(dl1))
			l--;
		return l;
	}

	/* shift down until exp = 0 or l = 0 */
	if (exp < 0 && exp > -64 && l)
		l >>= -exp;
	else
		return (0);

	return (SIGND (dl1) ? -l : l);
}

/* convert double to unsigned int */
unsigned long __fixunsdfsi (double a1){
	union double_long dl1;
	int exp;
	unsigned long l;

	dl1.d = a1;

	if (!dl1.l.upper && !dl1.l.lower)
		return (0);

	exp = EXPD (dl1) - EXCESSD - 32;
	l = (((((dl1.l.upper) & 0xFFFFF) | HIDDEND) << 11) | (dl1.l.lower >> 21));

	if (exp > 0)
		return (0xFFFFFFFFul);	/* largest integer */

	/* shift down until exp = 0 or l = 0 */
	if (exp < 0 && exp > -32 && l)
		l >>= -exp;
	else
		return (0);

	return (l);
}

/* convert double to unsigned long long */
unsigned long long __fixunsdfdi (double a1){
	union double_long dl1;
	int exp;
	unsigned long long l;

	dl1.d = a1;

	if (dl1.ll == 0)
		return (0);

	exp = EXPD (dl1) - EXCESSD - 64;

	l = dl1.ll;

	if (exp > 0)
		return (unsigned long long)-1;

	/* shift down until exp = 0 or l = 0 */
	if (exp < 0 && exp > -64 && l)
		l >>= -exp;
	else
		return (0);

	return (l);
}

/* addtwo doubles */
double __adddf3 (double a1, double a2){
	long long mant1, mant2;
	union double_long fl1, fl2;
	int exp1, exp2;
	int sign = 0;

	fl1.d = a1;
	fl2.d = a2;

	/* check for zero args */
	if (!fl2.ll)
		goto test_done;
	if (!fl1.ll) {
		fl1.d = fl2.d;
		goto test_done;
	}

	exp1 = EXPD(fl1);
	exp2 = EXPD(fl2);

	if (exp1 > exp2 + 54)
		goto test_done;
	if (exp2 > exp1 + 54) {
		fl1.d = fl2.d;
		goto test_done;
	}

	/* do everything in excess precision so's we can round later */
	mant1 = MANTD_LL(fl1) << 9;
	mant2 = MANTD_LL(fl2) << 9;

	if (SIGND(fl1))
		mant1 = -mant1;
	if (SIGND(fl2))
		mant2 = -mant2;

	if (exp1 > exp2)
		mant2 >>= exp1 - exp2;
	else {
		mant1 >>= exp2 - exp1;
		exp1 = exp2;
	}
	mant1 += mant2;

	if (mant1 < 0) {
		mant1 = -mant1;
		sign = SIGNBIT;
	} else if (!mant1) {
		fl1.d = 0;
		goto test_done;
	}

	/* normalize up */
	while (!(mant1 & ((long long)7<<61))) {
		mant1 <<= 1;
		exp1--;
	}

	/* normalize down? */
	if (mant1 & ((long long)3<<62)) {
		mant1 >>= 1;
		exp1++;
	}

	/* round to even */
	mant1 += (mant1 & (1<<9)) ? (1<<8) : ((1<<8)-1);

	/* normalize down? */
	if (mant1 & ((long long)3<<62)) {
		mant1 >>= 1;
		exp1++;
	}

	/* lose extra precision */
	mant1 >>= 9;

	/* turn off hidden bit */
	mant1 &= ~HIDDEND_LL;

	/* pack up and go home */
	fl1.ll = PACKD_LL(sign,exp1,mant1);

test_done:
	return (fl1.d);
}

/* subtract two doubles */
double __subdf3 (double a1, double a2){
	union double_long fl1, fl2;

	fl1.d = a1;
	fl2.d = a2;

	/* check for zero args */
	if (!fl2.ll)
		return (fl1.d);
	/* twiddle sign bit and add */
	fl2.l.upper ^= SIGNBIT;
	if (!fl1.ll)
		return (fl2.d);
	return __adddf3 (a1, fl2.d);
}

/* multiply two doubles */
double __muldf3 (double a1, double a2){
	union double_long fl1, fl2;
	unsigned long long result;
	int exp;
	int sign;

	fl1.d = a1;
	fl2.d = a2;

	if (!fl1.ll || !fl2.ll) {
		fl1.d = 0;
		goto test_done;
	}

	/* compute sign and exponent */
	sign = SIGND(fl1) ^ SIGND(fl2);
	exp = EXPD(fl1) - EXCESSD;
	exp += EXPD(fl2);

	fl1.ll = MANTD_LL(fl1);
	fl2.ll = MANTD_LL(fl2);

	/* the multiply is done as one 31x31 multiply and two 31x21 multiples */
	result = (fl1.ll >> 21) * (fl2.ll >> 21);
	result += ((fl1.ll & 0x1FFFFF) * (fl2.ll >> 21)) >> 21;
	result += ((fl2.ll & 0x1FFFFF) * (fl1.ll >> 21)) >> 21;

	result >>= 2;
	if (result & ((long long)1<<61)) {
		/* round */
		result += 1<<8;
		result >>= 9;
	} else {
		/* round */
		result += 1<<7;
		result >>= 8;
		exp--;
	}
	if (result & (HIDDEND_LL<<1)) {
		result >>= 1;
		exp++;
	}

	result &= ~HIDDEND_LL;

	/* pack up and go home */
	fl1.ll = PACKD_LL(sign,exp,result);
test_done:
	return (fl1.d);
}

/* divide two doubles */
double __divdf3 (double a1, double a2){
	union double_long fl1, fl2;
	long long mask,result;
	int exp, sign;

	fl1.d = a1;
	fl2.d = a2;

	/* subtract exponents */
	exp = EXPD(fl1) - EXPD(fl2) + EXCESSD;

	/* compute sign */
	sign = SIGND(fl1) ^ SIGND(fl2);

	/* numerator zero??? */
	if (fl1.ll == 0) {
		/* divide by zero??? */
		if (fl2.ll == 0)
			fl1.ll = ((unsigned long long)1<<63)-1;	/* NaN */
		else
			fl1.ll = 0;
		goto test_done;
	}

	/* return +Inf or -Inf */
	if (fl2.ll == 0) {
		fl1.ll = PACKD_LL(SIGND(fl1),2047,0);
		goto test_done;
	}


	/* now get mantissas */
	fl1.ll = MANTD_LL(fl1);
	fl2.ll = MANTD_LL(fl2);

	/* this assures we have 54 bits of precision in the end */
	if (fl1.ll < fl2.ll) {
		fl1.ll <<= 1;
		exp--;
	}

	/* now we perform repeated subtraction of fl2.ll from fl1.ll */
	mask = (long long)1<<53;
	result = 0;
	while (mask) {
		if (fl1.ll >= fl2.ll){
			result |= mask;
			fl1.ll -= fl2.ll;
		}
		fl1.ll <<= 1;
		mask >>= 1;
	}

	/* round */
	result += 1;

	/* normalize down */
	exp++;
	result >>= 1;

	result &= ~HIDDEND_LL;

	/* pack up and go home */
	fl1.ll = PACKD_LL(sign, exp, result);

test_done:
	return (fl1.d);
}

int __gtdf2 (double a1, double a2){
	return __cmpdf2 ((float) a1, (float) a2) > 0;
}

int __gedf2 (double a1, double a2){
	return (__cmpdf2 ((float) a1, (float) a2) >= 0) - 1;
}

int __ltdf2 (double a1, double a2){
	return - (__cmpdf2 ((float) a1, (float) a2) < 0);
}

int __ledf2 (double a1, double a2){
	return __cmpdf2 ((float) a1, (float) a2) > 0;
}

int __eqdf2 (double a1, double a2){
	union double_long d1, d2;

	d1.d = a1;
	d2.d = a2;

	return !(d1.ll == d2.ll);
}

int __nedf2 (double a1, double a2){
	union double_long d1, d2;

	d1.d = a1;
	d2.d = a2;

	return (d1.ll != d2.ll);
}
