#include <hf-risc.h>

int main(void){
	int i;
	char buf[50] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
	
	spi_setup();
	
	spi_start();
	for (i = 0; i < 10; i++)
		buf[i] = spi_sendrecv(buf[i]);
	spi_stop();
	
	return 0;
}
