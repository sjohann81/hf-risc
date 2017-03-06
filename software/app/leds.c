#include <hf-risc.h>

uint32_t extio_out = 0x01;

int main(void){
	
	for(;;){
		EXTIO_OUT = extio_out;
		extio_out <<= 1;
		if (extio_out == 0x10) extio_out = 0x01;
		delay_ms(500);
	}
		
	return 0;
}
