#include <hf-risc.h>
#include "assoc.h"

int main(void){
	SYNC_MAC_OPER = 0xaabbccdd;
	printf("SYNC_MAC_RESULT: %08x\n", SYNC_MAC_RESULT);
	SYNC_MAC_OPER ^= -1;
	SYNC_MAC_RST ^= -1;
	ASYNC_MAC_OPER = 0xddbbccaa;
	printf("ASYNC_MAC_RESULT: %08x\n", ASYNC_MAC_RESULT);
	ASYNC_MAC_OPER ^= -1;
	ASYNC_MAC_RST ^= -1;

	DE_PAUSE ^= -1;
	DE_CONFIG ^= -1;
	DE_CDE_SEL ^= -1;
	DE_MDE_SEL_L ^= -1;
	DE_MDE_SEL_B ^= -1;
	DE_CDE_CTRL ^= -1;

	DE_PAUSE ^= -1;
	DE_CONFIG ^= -1;
	DE_CDE_SEL ^= -1;
	DE_MDE_SEL_L ^= -1;
	DE_MDE_SEL_B ^= -1;
	DE_CDE_CTRL ^= -1;

	return 0;
}
