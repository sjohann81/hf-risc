#include <hf-risc.h>

#define WRAPPER_BASE			0xe2000000
#define WRAPPER_CONTROL			(*(volatile uint32_t *)(WRAPPER_BASE + 0x000))
#define WRAPPER_DATA0			(*(volatile uint32_t *)(WRAPPER_BASE + 0x010))
#define WRAPPER_DATA1			(*(volatile uint32_t *)(WRAPPER_BASE + 0x020))
#define WRAPPER_DATA2			(*(volatile uint32_t *)(WRAPPER_BASE + 0x030))
#define WRAPPER_DATA3			(*(volatile uint32_t *)(WRAPPER_BASE + 0x040))

int main(void){
	WRAPPER_CONTROL = 0x12345678;

	printf("WRAPPER_CONTROL: %08x\n", WRAPPER_CONTROL);
	
	WRAPPER_DATA0 = 0x11223344;
	WRAPPER_DATA1 = 0x55667788;
	WRAPPER_DATA2 = 0x44332211;
	WRAPPER_DATA3 = 0xb16b00b5;

	printf("WRAPPER_DATA0: %08x\n", WRAPPER_DATA0);
	printf("WRAPPER_DATA1: %08x\n", WRAPPER_DATA1);
	printf("WRAPPER_DATA2: %08x\n", WRAPPER_DATA2);
	printf("WRAPPER_DATA3: %08x\n", WRAPPER_DATA3);

	return 0;
}
