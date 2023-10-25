#include <hf-risc.h>

#define VGA_BASE			0xe2800000
#define VGA_STATUS			(*(volatile uint32_t *)(VGA_BASE + 0x0010))
#define VGA_ADDRESS			(*(volatile uint32_t *)(VGA_BASE + 0x0020))
#define VGA_DATA			(*(volatile uint32_t *)(VGA_BASE + 0x0030))

#define VGA_NOTBUSY			(1 << 0)
#define VGA_WRITE			(1 << 1)

#define VGA_WIDTH			300
#define VGA_HEIGHT			218

void vram_putbyte(uint16_t addr, char color)
{
	VGA_ADDRESS = addr;
	VGA_DATA = color;
	VGA_STATUS |= VGA_WRITE;
	while (!(VGA_STATUS & VGA_NOTBUSY));
	VGA_STATUS &= ~VGA_WRITE;
}

static char vram_getbyte(uint16_t addr)
{
	char data1, data2;
	
	VGA_ADDRESS = addr;
	
retry:
	while (!(VGA_STATUS & VGA_NOTBUSY));
	data1 = VGA_DATA;
	while (!(VGA_STATUS & VGA_NOTBUSY));
	data2 = VGA_DATA;
	
	if (data1 != data2) goto retry;
	
	return data1;
}

void display_pixel(uint16_t x, uint16_t y, char color)
{
	char byte;
	uint16_t vram_addr;
	
	vram_addr = y * (VGA_WIDTH >> 1) + (x >> 1);
	color &= 0xf;	
	byte = vram_getbyte(vram_addr);
	
	if (x & 1) {
		byte &= 0x0f;
		byte |= color << 4;
	} else {
		byte &= 0xf0;
		byte |= color;
	}

	vram_putbyte(vram_addr, byte);
}

int main(void){
	char buf[100];
	uint16_t x, y, addr;
	char color;

	for (y = 0; y < VGA_HEIGHT; y++)
		for (x = 0; x < VGA_WIDTH; x++)
			display_pixel(x, y, 0);

	vram_putbyte(200, 0);
	vram_putbyte(201, 1);
	vram_putbyte(202, 2);
	vram_putbyte(203, 3);
	vram_putbyte(204, 4);
	vram_putbyte(205, 5);
	vram_putbyte(206, 6);
	vram_putbyte(207, 7);
	vram_putbyte(208, 8);
	vram_putbyte(209, 9);
	vram_putbyte(210, 10);
	vram_putbyte(211, 11);
	vram_putbyte(212, 12);
	vram_putbyte(213, 13);
	vram_putbyte(214, 14);
	vram_putbyte(215, 15);

	while (1) {
		printf("pos x:\n");
		gets(buf);
		x = atoi(buf);
		printf("pos y:\n");
		gets(buf);
		y = atoi(buf);
		printf("color:\n");
		gets(buf);
		color = atoi(buf);
		
		addr = y * (VGA_WIDTH >> 1) + (x >> 1);
		printf("byte was: %02x\n", vram_getbyte(addr));
		display_pixel(x, y, color);
		printf("byte now: %02x\n", vram_getbyte(addr));
	}

	return 0;
}
