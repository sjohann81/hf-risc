/* bootloader.c */
#include <hf-risc.h>

int32_t main(void){
	void (*funcPtr)();

	printf("boot\n");

//	uint32_t *ptr, *ptr2;

//	ptr = (uint32_t *)RAM_BASE;
//	ptr2 = (uint32_t *)EXT_SRAM;
//	memcpy(ptr2, ptr, 4096);

//	funcPtr = (void (*)(void *))(EXT_SRAM);

	funcPtr = (void (*)(void *))(RAM_BASE);
	funcPtr();

	return 0;
}

