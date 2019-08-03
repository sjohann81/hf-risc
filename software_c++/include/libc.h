/* file:          libc.c
 * description:   small C library prototypes
 * date:          09/2015
 * author:        Sergio Johann Filho <sergio.filho@pucrs.br>
 */

#include <stdarg.h>

/*
constants, tests and transformations
*/
#define NULL			((void *)0)
#define USED			1
#define TRUE			1
#define FALSE			0
#define isprint(c)		(' '<=(c)&&(c)<='~')
#define isspace(c)		((c)==' '||(c)=='\t'||(c)=='\n'||(c)=='\r')
#define isdigit(c)		('0'<=(c)&&(c)<='9')
#define islower(c)		('a'<=(c)&&(c)<='z')
#define isupper(c)		('A'<=(c)&&(c)<='Z')
#define isalpha(c)		(islower(c)||isupper(c))
#define isalnum(c)		(isalpha(c)||isdigit(c))
#define min(a,b)		((a)<(b)?(a):(b))

extern unsigned long _stack_start;  /* Start of the STACK memory. */
extern unsigned long _stack_end;    /* End of the STACK memory. */
extern unsigned long _heap_start;   /* Start of the HEAP memory. */
extern unsigned long _heap_end;     /* End of the HEAP memory (one byte past the last byte of this memory). */
extern unsigned long _sidata;       /* Start address for the contents initialization of the .data
                                       section. defined in linker script. */
extern unsigned long _sdata;        /* Start address for the .data section, defined in linker script */
extern unsigned long _edata;        /* End address for the .data section, defined in linker script. */
extern unsigned long _sbss;         /* Start address for the .bss section, defined in linker script. */
extern unsigned long _ebss;         /* End address for the .bss section, defined in linker script. */
extern unsigned long _end;          /* Start address of the heap memory, defined in linker script. */

#if BIG_ENDIAN

#define htons(n) (n)
#define ntohs(n) (n)
#define htonl(n) (n)
#define ntohl(n) (n)

#else

#define htons(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))
#define ntohs(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))

#define htonl(n) (((((uint32_t)(n) & 0xFF)) << 24) | \
                  ((((uint32_t)(n) & 0xFF00)) << 8) | \
                  ((((uint32_t)(n) & 0xFF0000)) >> 8) | \
                  ((((uint32_t)(n) & 0xFF000000)) >> 24))

#define ntohl(n) (((((uint32_t)(n) & 0xFF)) << 24) | \
                  ((((uint32_t)(n) & 0xFF00)) << 8) | \
                  ((((uint32_t)(n) & 0xFF0000)) >> 8) | \
                  ((((uint32_t)(n) & 0xFF000000)) >> 24))
#endif

/*
custom C library
*/
void putchar(int32_t value);
int32_t kbhit(void);
int32_t getchar(void);
int8_t *strcpy(int8_t *dst, const int8_t *src);
int8_t *strncpy(int8_t *s1, int8_t *s2, int32_t n);
int8_t *strcat(int8_t *dst, const int8_t *src);
int8_t *strncat(int8_t *s1, int8_t *s2, int32_t n);
int32_t strcmp(const int8_t *s1, const int8_t *s2);
int32_t strncmp(int8_t *s1, int8_t *s2, int32_t n);
int8_t *strstr(const int8_t *string, const int8_t *find);
int32_t strlen(const int8_t *s);
int8_t *strchr(const int8_t *s, int32_t c);
int8_t *strpbrk(int8_t *str, int8_t *set);
int8_t *strsep(int8_t **pp, int8_t *delim);
int8_t *strtok(int8_t *s, const int8_t *delim);
void *memcpy(void *dst, const void *src, uint32_t n);
void *memmove(void *dst, const void *src, uint32_t n);
int32_t memcmp(const void *cs, const void *ct, uint32_t n);
void *memset(void *s, int32_t c, uint32_t n);
int32_t strtol(const int8_t *s, int8_t **end, int32_t base);
int32_t atoi(const int8_t *s);
float atof(const int8_t *p);
int32_t ftoa(float f, int8_t *outbuf, int32_t precision);
int8_t *itoa(int32_t i, int8_t *s, int32_t base);
int32_t puts(const int8_t *str);
int8_t *gets(int8_t *s);
int32_t abs(int32_t n);
int32_t random(void);
void srand(uint32_t seed);
int32_t printf(const int8_t *fmt, ...);
int32_t sprintf(int8_t *out, const int8_t *fmt, ...);

/*
auxiliary routines
*/
void uart_init(uint32_t baud);
void delay_ms(uint32_t msec);
void delay_us(uint32_t usec);
void panic(void);

/* IEEE single-precision definitions */
#define SNG_EXPBITS	8
#define SNG_FRACBITS	23
#define SNG_EXP_BIAS	127
#define SNG_EXP_INFNAN	255
#define EXCESS		126
#define SIGNBIT		0x80000000
#define HIDDEN		(1 << 23)
#define SIGN(fp)	((fp) & SIGNBIT)
#define EXP(fp)		(((fp) >> 23) & 0xFF)
#define MANT(fp)	(((fp) & 0x7FFFFF) | HIDDEN)
#define PACK(s,e,m)	((s) | ((e) << 23) | (m))

union float_long{
	float f;
	int32_t l;
	uint32_t u;
};

/* the following deal with IEEE double-precision numbers */
#define EXCESSD		1022
#define HIDDEND		(1 << 20)
#define EXPD(fp)	(((fp.l.upper) >> 20) & 0x7FF)
#define SIGND(fp)	((fp.l.upper) & SIGNBIT)
#define MANTD(fp)	(((((fp.l.upper) & 0xFFFFF) | HIDDEND) << 10) | (fp.l.lower >> 22))
#define HIDDEND_LL	((long long)1 << 52)
#define MANTD_LL(fp)	((fp.ll & (HIDDEND_LL-1)) | HIDDEND_LL)
#define PACKD_LL(s,e,m)	(((long long)((s)+((e)<<20))<<32)|(m))

union double_long {
	double d;
	struct {
#ifdef LITTLE_ENDIAN
		uint32_t lower;
		int32_t upper;
#else
		int32_t upper;
		uint32_t lower;
#endif
	} l;
	uint64_t ll;
};
