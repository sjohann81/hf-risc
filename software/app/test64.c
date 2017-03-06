#include <hf-risc.h>

union big{
	struct{
		uint32_t high;
		uint32_t low;
	} l;
	uint64_t unum;
	int64_t num;
};

int main(){
	int32_t i, j;
	uint64_t a, b;
	int64_t c, d;
	union big stuff;

	printf("test unsigned multiply...\n");
	a = 0x89abcdef;
	b = 3;
	for (i = 0; i < 20; i++){
		stuff.unum = a * b;
		printf("%x * %x = %x%x\n", (uint32_t)a, (uint32_t)b, stuff.l.high, stuff.l.low);
		b *= 3;
	}

	printf("test unsigned divide...\n");
	a = stuff.unum;
	for (i = 0; i < 20; i++){
		stuff.unum = a / b;
		printf("%x%x / %x%x = %x%x\n", (uint32_t)(a >> 32), (uint32_t)(a & 0xffffffff), (uint32_t)(b >> 32), (uint32_t)(b & 0xffffffff), stuff.l.high, stuff.l.low);
		b /= 3;
	}

	printf("test signed multiply...\n");
	c = 0x89abcdef;
	d = -3;
	for (i = 0; i < 20; i++){
		stuff.num = c * d;
		printf("%x * %x = %x%x\n", (uint32_t)c, (uint32_t)d, stuff.l.high, stuff.l.low);
		d *= 3;
	}

	printf("test signed divide...\n");
	c = stuff.num | 1LL << 63;
	for (i = 0; i < 20; i++){
		stuff.unum = c / d;
		printf("%x%x / %x%x = %x%x\n", (uint32_t)(c >> 32), (uint32_t)(c & 0xffffffff), (uint32_t)(d >> 32), (uint32_t)(d & 0xffffffff), stuff.l.high, stuff.l.low);
		d /= 3;
	}

	stuff.unum = 1;

	printf("testing left shifts..\n");
	for (i = 0; i < 64; i++){
		printf("%x %x\n", stuff.l.high, stuff.l.low);
		stuff.unum <<= 1;
	}

	stuff.unum = 1LL << 63;

	printf("testing right (logical) shifts..\n");
	for (i = 0; i < 64; i++){
		printf("%x %x\n", stuff.l.high, stuff.l.low);
		stuff.unum >>= 1;
	}

	stuff.unum = 1LL << 63;

	printf("testing right (arithmetic) shifts..\n");
	for (i = 0; i < 64; i++){
		printf("%x %x\n", stuff.l.high, stuff.l.low);
		stuff.num >>= 1;
	}

	return 0;
}

