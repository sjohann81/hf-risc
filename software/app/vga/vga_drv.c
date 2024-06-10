#include <hf-risc.h>
#include "vga_drv.h"
#include "IBM_VGA_8x8.h"

#define VGA_BASE			0xe2800000
#define VGA_STATUS			(*(volatile uint32_t *)(VGA_BASE + 0x0010))
#define VGA_ADDRESS			(*(volatile uint32_t *)(VGA_BASE + 0x0020))
#define VGA_DATA			(*(volatile uint32_t *)(VGA_BASE + 0x0030))

#define VGA_NOTBUSY			(1 << 0)
#define VGA_WRITE			(1 << 1)


static void vram_putbyte(uint16_t addr, char color)
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

void display_quad(uint16_t x, uint16_t y, char color)
{
	uint16_t vram_addr1, vram_addr2;
	
	vram_addr1 = y * (VGA_WIDTH >> 1) + (x >> 1);
	vram_addr2 = vram_addr1 + (VGA_WIDTH >> 1);
	vram_putbyte(vram_addr1, color << 4 | color);
	vram_putbyte(vram_addr2, color << 4 | color);
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

char display_getpixel(uint16_t x, uint16_t y)
{
	char byte;
	uint16_t vram_addr;
	
	vram_addr = y * (VGA_WIDTH >> 1) + (x >> 1);	
	byte = vram_getbyte(vram_addr);
	
	if (x & 1)
		return (byte >> 4) & 0xf;
	else
		return byte & 0xf;
}

void display_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	int x = x1 - x0;
	int y = y1 - y0;
	int dx = abs(x);
	int sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y);
	int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	
	for (;;) {
		display_pixel(x0, y0, color);
		e2 = 2 * err;
		if (e2 >= dy) {
			if (x0 == x1)
				break;
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			if (y0 == y1) break;
			err += dx;
			y0 += sy;
		}
	}
}

void display_hline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color)
{
	int i;

	for (i = 0; i < length; i++)
		display_pixel(x0 + i, y0, color);
}

void display_vline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color)
{
	int i;
	
	for (i = 0; i < length; i++)
		display_pixel(x0, y0 + i, color);
}

void display_rectangle(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color)
{
	display_hline(x0, y0, width, color);
	display_hline(x0, y0 + height, width + 1, color);
	display_vline(x0, y0, height, color);
	display_vline(x0 + width, y0, height, color);
}

void display_frectangle(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color)
{
	int i;
	
	for (i = 0; i < height; i++)
		display_hline(x0, y0 + i, width, color);
}

void display_background(uint16_t color)
{
/*	display_frectangle(0, 0, VGA_WIDTH, VGA_HEIGHT, color);*/
	int i;
	char c;
	
	c = color & 0xf;
	c = c << 4 | c;
	
	for (i = 0; i < (VGA_WIDTH * VGA_HEIGHT) >> 1; i++)
		vram_putbyte(i, c);
}

void display_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
	int x = -r, y = 0, err = 2 - 2 * r, e2;
	
	do {
		display_pixel(x0 - x, y0 + y, color);
		display_pixel(x0 + x, y0 + y, color);
		display_pixel(x0 + x, y0 - y, color);
		display_pixel(x0 - x, y0 - y, color);
		
		e2 = err;
		if (e2 <= y) {
			err += ++y * 2 + 1;
			if (-x == y && e2 <= x)
				e2 = 0;
		}
		if (e2 > x)
			err += ++x * 2 + 1;
	} while (x <= 0);
}

void display_fcircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
	int x = -r, y = 0, err = 2 - 2 * r, e2;
	
	do {
		display_vline(x0 - x, y0 - y, 2 * y, color);
		display_vline(x0 + x, y0 - y, 2 * y, color);

		e2 = err;
		if (e2 <= y) {
			err += ++y * 2 + 1;
			if (-x == y && e2 <= x)
				e2 = 0;
		}
		if (e2 > x)
			err += ++x * 2 + 1;
	} while (x <= 0);
}

void display_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	display_line(x0, y0, x1, y1, color);
	display_line(x1, y1, x2, y2, color);
	display_line(x2, y2, x0, y0, color);
}

void display_ftriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	int16_t a, b, y, last;
	int16_t dx01, dy01, dx02, dy02, dx12, dy12;
	int32_t sa = 0, sb = 0;

	if (y0 > y1) {
		_swap_int16_t(y0, y1);
		_swap_int16_t(x0, x1);
	}
	if (y1 > y2) {
		_swap_int16_t(y2, y1);
		_swap_int16_t(x2, x1);
	}
	if (y0 > y1) {
		_swap_int16_t(y0, y1);
		_swap_int16_t(x0, x1);
	}

	if (y0 == y2) {
		a = b = x0;
		if (x1 < a)
			a = x1;
		else if (x1 > b)
			b = x1;
		if (x2 < a)
			a = x2;
		else if (x2 > b)
			b = x2;
		display_hline(a, y0, b - a + 1, color);

		return;
	}

	dx01 = x1 - x0;
	dy01 = y1 - y0;
	dx02 = x2 - x0;
	dy02 = y2 - y0;
	dx12 = x2 - x1;
	dy12 = y2 - y1;

	last = y1 == y2 ? y1 : y1 - 1;

	for (y = y0; y <= last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		if (a > b)
			_swap_int16_t(a, b);
		display_hline(a, y, b - a + 1, color);
	}

	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if (a > b)
			_swap_int16_t(a, b);
		display_hline(a, y, b - a + 1, color);
	}
}

void display_char(uint8_t ascii, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor)
{
	uint8_t c, b;
	int i;
	
	for (i = 0; i < FONT_Y; i++) {
		c = ibm_8x8[ascii][i];
		for (b = 0; b < 8; b++) {
			if ((c >> b) & 0x01)
				display_frectangle(x0 + (8 - b) * size, y0 + i * size, size, size, fgcolor);
		}
	}
}

void display_print(char *string, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor)
{
	while (*string) {
		display_char(*string, x0, y0, size, fgcolor);
		string++;

		if (x0 < VGA_WIDTH - 1 - (FONT_X + FONT_X) * size)
			x0 += FONT_X * size;
	}
}
