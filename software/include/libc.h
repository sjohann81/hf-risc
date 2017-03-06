/* file:          libc.c
 * description:   small C library prototypes
 * date:          09/2015
 * author:        Sergio Johann Filho <sergio.filho@pucrs.br>
 */

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
#define ntohs(A)		(((A)>>8) | (((A)&0xff)<<8))
#define ntohl(A)		(((A)>>24) | (((A)&0xff0000)>>8) | (((A)&0xff00)<<8) | ((A)<<24))

/*
custom C library
*/
extern void putchar(int32_t value);
extern int32_t kbhit(void);
extern int32_t getchar(void);
extern int8_t *strcpy(int8_t *dst, const int8_t *src);
extern int8_t *strncpy(int8_t *s1, int8_t *s2, int32_t n);
extern int8_t *strcat(int8_t *dst, const int8_t *src);
extern int8_t *strncat(int8_t *s1, int8_t *s2, int32_t n);
extern int32_t strcmp(const int8_t *s1, const int8_t *s2);
extern int32_t strncmp(int8_t *s1, int8_t *s2, int32_t n);
extern int8_t *strstr(const int8_t *string, const int8_t *find);
extern int32_t strlen(const int8_t *s);
extern int8_t *strchr(const int8_t *s, int32_t c);
extern int8_t *strpbrk(int8_t *str, int8_t *set);
extern int8_t *strsep(int8_t **pp, int8_t *delim);
extern int8_t *strtok(int8_t *s, const int8_t *delim);
extern void *memcpy(void *dst, const void *src, uint32_t n);
extern void *memmove(void *dst, const void *src, uint32_t n);
extern int32_t memcmp(const void *cs, const void *ct, uint32_t n);
extern void *memset(void *s, int32_t c, uint32_t n);
extern int32_t strtol(const int8_t *s, int8_t **end, int32_t base);
extern int32_t atoi(const int8_t *s);
extern float atof(const int8_t *p);
extern int8_t *itoa(int32_t i, int8_t *s, int32_t base);
extern int32_t puts(const int8_t *str);
extern int8_t *gets(int8_t *s);
extern int32_t abs(int32_t n);
extern int32_t random(void);
extern void srand(uint32_t seed);
extern int32_t printf(const int8_t *fmt, ...);
extern int32_t sprintf(int8_t *out, const int8_t *fmt, ...);

/*
auxiliary routines / interrupt management
*/
extern void uart_init(uint32_t baud);
extern void delay_ms(uint32_t msec);
extern void delay_us(uint32_t usec);
void interrupt_handler(uint32_t cause, uint32_t *stack);
void interrupt_register(uint32_t mask, funcptr ptr);


