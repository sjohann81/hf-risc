/* bootloader.c */
#include <hf-risc.h>

int32_t main(void){
	funcptr funcPtr;

	uart_init(57600);

/*	printf("\n=====================================================================");
	printf("\nWelcome to the HF-RISC Bootloader - ");
	printf(__DATE__); printf(" "); printf(__TIME__);
	printf("\nEmbedded Systems Group - GSE, PUCRS - [2006 - 2015]");
	printf("\n=====================================================================\n");
*/
	printf("boot\n");

	funcPtr = (funcptr)ADDR_RAM_BASE;
	funcPtr();

	return 0;
}

