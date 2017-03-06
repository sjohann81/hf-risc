#include <hf-risc.h>
#include <mcp23s17.h>

int main(void){
	uint8_t a, b;
	
	mcp23s17_init(0, ALL_OUTPUTS, ALL_OUTPUTS);
	mcp23s17_dir(0, 0, ALL_INPUTS);
	mcp23s17_dir(0, 1, ALL_INPUTS);
	a = mcp23s17_read(0, 0);
	b = mcp23s17_read(0, 1);
	mcp23s17_dir(0, 0, ALL_OUTPUTS);
	mcp23s17_dir(0, 1, ALL_OUTPUTS);
	mcp23s17_write(0, 0, 0xff);
	mcp23s17_write(0, 1, 0xff);
	
	return 0;
}
