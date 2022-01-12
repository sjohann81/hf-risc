#include <hf-risc.h>
#include "display_drv.h"
#include "display_font.h"

struct display_s display;
struct display_s *display_p = &display;


void display_init(uint16_t width, uint16_t height, uint16_t scale)
{
	display_p->ptr = (uint16_t *)&DISPLAY_RAM;
	display_p->width = width;
	display_p->height = height;
	display_p->scale = scale;
	
	DISPLAY_WIDTH = display_p->width;
	DISPLAY_HEIGHT = display_p->height;
	DISPLAY_SCALE = display_p->scale;
	DISPLAY_STATUS |= DISPLAY_INITIALIZE;
	
	memset(display_p->ptr, 0xff, display_p->width * display_p->height * sizeof(uint16_t));
}

void display_blit(void)
{
	DISPLAY_STATUS |= DISPLAY_UPDATE;
}

void display_pixel(uint16_t x0, uint16_t y0, uint16_t color)
{
	display_p->ptr[y0 * display_p->width + x0] = color;
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
		display_p->ptr[y0 * display_p->width + x0 + i] = color;
}

void display_vline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color)
{
	int i;
	
	for (i = 0; i < length; i++)
		display_p->ptr[(y0 + i) * display_p->width + x0] = color;
}

void display_rectangle(uint16_t x0, uint16_t y0, uint16_t length, uint16_t width, uint16_t color)
{
	display_hline(x0, y0, length, color);
	display_hline(x0, y0 + width, length, color);
	display_vline(x0, y0, width, color);
	display_vline(x0 + length, y0, width, color);
}

void display_frectangle(uint16_t x0, uint16_t y0, uint16_t length, uint16_t width, uint16_t color)
{
	int i;
	
	for (i = 0; i < width; i++)
		display_hline(x0, y0 + i, length, color);
}

void display_background(uint16_t color)
{
	display_frectangle(0, 0, display_p->width - 1, display_p->height - 1, color);
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

void display_char(uint8_t ascii, uint16_t x0, uint16_t y0,uint16_t size, uint16_t fgcolor)
{
	uint8_t c, b;
	int i;
	
	if ((ascii >= 32) && (ascii < 128)) {
		for (i = 0; i < FONT_X; i++) {
			c = font[ascii][i];
			for (b = 0; b < 8; b++) {
				if((c >> b) & 0x01)
					display_frectangle(x0 + i * size, y0 + b * size, size, size, fgcolor);
			}
		}
	}
}

void display_print(char *string, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor)
{
	while (*string) {
		display_char(*string, x0, y0, size, fgcolor);
		string++;

		if (x0 < display_p->width - 1 - ((FONT_X + FONT_SPACE) * size))
			x0 += FONT_SPACE * size;
	}
}
