#include <hf-risc.h>
#include "vga_drv.h"

void display_test0()
{
	int i, j, k, size = 1;
	
	for (j = 10, k = 0; k < 255; j += FONT_Y * size) {
		for (i = 1; i < VGA_WIDTH - 1 - FONT_X * size && k < 255; i += FONT_X * size, k++) {
			display_char(k, i, j, size, YELLOW);
		}
	}
	
	display_print("The quick brown fox", 0, 100, 1, GREEN);
	display_print("jumps over the lazy dog", 0, 150 + FONT_Y, 1, GREEN);
}


void display_test1()
{
	char buf[30];
		
	display_pixel(10, 10, YELLOW);
	display_line(0, 0, VGA_WIDTH-1, VGA_HEIGHT-1, GREEN);
	display_line(VGA_WIDTH-1, 0, 0, VGA_HEIGHT-1, GREEN);
	display_rectangle(0, 0, 120, 120, LGREEN);
	display_frectangle(20, 10, 50, 10, RED);
	display_circle(45,90,15, WHITE);
	display_fcircle(25, 165, 20, BLUE);
	display_ftriangle(50, 60, 10, 125, 80, 75, YELLOW);
	display_triangle(100, 65, 50, 175, 150, 175, MAGENTA);
	display_char('a', 5, 80, 4, RED);
	display_print("abcdefgh", 180, 0, 1, GREEN);
	ftoa(-123.456, buf, 4);
	display_print(buf, 120, 60, 2, BLUE);
	itoa(456, buf, 10);
	display_print(buf, 100, 80, 2, WHITE);
	itoa(0xdeadbeef, buf, 16);
	display_print(buf, 60, 100, 3, GRAY);
}

void display_test2()
{
	int i, j, k;
	
	for (j = 10, k = 0; k < 255; j += FONT_Y)
		for (i = 1; i < VGA_WIDTH - 1 - FONT_X && k < 255; i += FONT_X, k++)
			display_char(k, i, j, 1, YELLOW);

	display_print("The quick brown fox", 0, 100, 1, GREEN);
	display_print("jumps over the lazy dog", 0, 150 + FONT_Y, 1, GREEN);
}


int dot_demo()
{
	uint16_t k, l, c;
	int i = 0;
	
	for (;;) {
		k = random() % VGA_WIDTH;
		l = random() % VGA_HEIGHT;
		c = random() % 16;

		display_pixel(k, l, c);
		
		if (i > 100000)
			break;
			
		i++;
		
	}
	
	delay_ms(2000);
	
	return i;
}

int line_demo()
{
	uint16_t k, l, m, n, c;
	int i = 0;
	
	for (;;) {
		k = random() % VGA_WIDTH;
		l = random() % VGA_HEIGHT;
		m = random() % VGA_WIDTH;
		n = random() % VGA_HEIGHT;
		c = random() % 16;

		display_line(k, l, m, n, c);
			
		delay_ms(20);
			
		if (i > 500)
			break;
			
		i++;
	}
	
	return i;
}


int rectangle_demo()
{
	uint16_t k, l, m, n, c;
	int i = 0;
	
	for (;;) {
		do {
			m = random() % (VGA_WIDTH / 4);
		} while (m < 5);
		do {
			n = random() % (VGA_HEIGHT / 4);
		} while (n < 5);

		k = random() % (VGA_WIDTH - m);
		l = random() % (VGA_HEIGHT - n);
		c = random() % 16;
		
		if (i & 1)
			display_rectangle(k, l, m, n, c);
		else
			display_frectangle(k, l, m, n, c);
		
		delay_ms(20);
		
		if (i > 500)
			break;
			
		i++;
	}
	
	return i;
}


int circle_demo()
{
	uint16_t k, l, r, c;
	int i = 0;
	
	for (;;) {
		do {
			r = random() % (VGA_WIDTH / 4);
		} while (r < 5);
		
		k = random() % (VGA_WIDTH - 2 * r) + r;
		l = random() % (VGA_HEIGHT - 2 * r) + r;
		c = random() % 16;

		if (i & 1)
			display_circle(k, l, r, c);
		else
			display_fcircle(k, l, r, c);
		
		delay_ms(20);
		
		if (i > 500)
			break;
			
		i++;
	}
	
	return i;
}


int triangle_demo()
{
	uint16_t k, l, m, n, o, p, c;
	int i = 0;
	
	for (;;) {
		k = random() % VGA_WIDTH;
		l = random() % VGA_HEIGHT;
		m = random() % VGA_WIDTH;
		n = random() % VGA_HEIGHT;
		o = random() % VGA_WIDTH;
		p = random() % VGA_HEIGHT;
		c = random() % 16;
			
		if (i & 1)
			display_triangle(k, l, m, n, o, p, c);
		else
			display_ftriangle(k, l, m, n, o, p, c);
		
		delay_ms(20);
		
		if (i > 500)
			break;
			
		i++;
	}
	
	return i;
}

void display_test3()
{
	display_background(BLACK);
	dot_demo();
	display_background(BLACK);
	line_demo();
	display_background(BLACK);
	rectangle_demo();
	display_background(BLACK);
	circle_demo();
	display_background(BLACK);
	triangle_demo();
}

int main(void)
{
	while (1) {
		display_background(BLACK);
		display_test1();
		delay_ms(5000);
		display_background(BLUE);
		display_test2();
		delay_ms(5000);
		display_test3();
	}

	return 0;
}
