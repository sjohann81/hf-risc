/* bootloader.c */
#include <hf-risc.h>

int32_t main(void){
	void (*funcPtr)();

	printf("boot\n");

	funcPtr = (void (*)(void *))(RAM_BASE);
	funcPtr();

	return 0;
}

