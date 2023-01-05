#include <hf-risc.h>
#include "display_drv.h"

void display_test1()
{
	char buf[30];
		
	display_pixel(10, 10, YELLOW);
	display_line(0, 0, display_p->width-1, display_p->height-1, GREEN);
	display_line(display_p->width-1, 0, 0, display_p->height-1, GREEN);
	display_rectangle(0, 0, 240, 240, LIME);
	display_frectangle(20, 10, 100, 20, RED);
	display_circle(95,170,15, WHITE);
	display_fcircle(25, 65, 20, BLUE);
	display_ftriangle(50, 60, 10, 125, 80, 75, YELLOW);
	display_triangle(200, 125, 100, 375, 300, 375, PURPLE);
	display_char('a', 5, 80, 4, RED);
	display_print("abcdefgh", 180, 0, 1, GREEN);
	ftoa(-123.456, buf, 4);
	display_print(buf, 120, 60, 2, BLUE);
	itoa(456, buf, 10);
	display_print(buf, 200, 80, 2, WHITE);
	itoa(0xdeadbeef, buf, 16);
	display_print(buf, 60, 100, 3, OLIVE);

	display_blit();
}

void display_test2()
{
	int i, j, k, size = 1;
	
	for (j = 10, k = 0; k < 255; j += FONT_Y * size) {
		for (i = 1; i < display_p->width - 1 - FONT_X * size && k < 255; i += FONT_X * size, k++) {
			display_char(k, i, j, size, YELLOW);
		}
	}
	
	size++;
	
	for (j = 60, k = 0; k < 255; j += FONT_Y * size) {
		for (i = 1; i < display_p->width - 1 - FONT_X * size && k < 255; i += FONT_X * size, k++) {
			display_char(k, i, j, size, YELLOW);
		}
	}
	
	
	display_print("The quick brown fox", 0, 300, 2, GREEN);
	display_print("jumps over the lazy dog", 0, 350 + FONT_Y, 2, GREEN);
	
	display_blit();
}


int dot_demo()
{
	uint16_t k, l, c;
	int i = 0;
	
	for (;;) {
		k = random() % display_p->width;
		l = random() % display_p->height;
		c = RGB((random() % 255), (random() % 255), (random() % 255));

		display_pixel(k, l, c);
		
		if (i > 1000)
			break;
			
		i++;
		
		display_blit();
	}
	
	display_background(BLACK);
	
	delay_ms(2000);
	
	i = 0;
	
	for (;;) {
		k = random() % display_p->width;
		l = random() % display_p->height;
		c = RGB((random() % 255), (random() % 255), (random() % 255));

		display_pixel(k, l, c);
		
		if (i > 1000)
			break;
			
		i++;
		
	}

	display_blit();
	
	delay_ms(2000);
	
	return i;
}

int line_demo()
{
	uint16_t k, l, m, n, c;
	int i = 0;
	
	for (;;) {
		k = random() % display_p->width;
		l = random() % display_p->height;
		m = random() % display_p->width;
		n = random() % display_p->height;
		c = RGB((random() % 255), (random() % 255), (random() % 255));

		display_line(k, l, m, n, c);
			
		delay_ms(20);
			
		if (i > 500)
			break;
			
		i++;
		
		display_blit();
	}
	
	return i;
}


int rectangle_demo()
{
	uint16_t k, l, m, n, c;
	int i = 0;
	
	for (;;) {
		do {
			m = random() % (display_p->width / 4);
		} while (m < 5);
		do {
			n = random() % (display_p->height / 4);
		} while (n < 5);

		k = random() % (display_p->width - m);
		l = random() % (display_p->height - n);
		c = RGB((random() % 255), (random() % 255), (random() % 255));
		
		if (i & 1)
			display_rectangle(k, l, m, n, c);
		else
			display_frectangle(k, l, m, n, c);
		
		delay_ms(20);
		
		if (i > 500)
			break;
			
		i++;
		
		display_blit();
	}
	
	return i;
}


int circle_demo()
{
	uint16_t k, l, r, c;
	int i = 0;
	
	for (;;) {
		do {
			r = random() % (display_p->height / 4);
		} while (r < 5);
		
		k = random() % (display_p->width - 2 * r) + r;
		l = random() % (display_p->height - 2 * r) + r;
		c = RGB((random() % 255), (random() % 255), (random() % 255));

		if (i & 1)
			display_circle(k, l, r, c);
		else
			display_fcircle(k, l, r, c);
		
		delay_ms(20);
		
		if (i > 500)
			break;
			
		i++;
		
		display_blit();
	}
	
	return i;
}


int triangle_demo()
{
	uint16_t k, l, m, n, o, p, c;
	int i = 0;
	
	for (;;) {
		k = random() % display_p->width;
		l = random() % display_p->height;
		m = random() % display_p->width;
		n = random() % display_p->height;
		o = random() % display_p->width;
		p = random() % display_p->height;
		c = RGB((random() % 255), (random() % 255), (random() % 255));
			
		if (i & 1)
			display_triangle(k, l, m, n, o, p, c);
		else
			display_ftriangle(k, l, m, n, o, p, c);
		
		delay_ms(20);
		
		if (i > 500)
			break;
			
		i++;
		
		display_blit();
	}
	
	return i;
}

void display_test3()
{
//	while (1) {
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
//	}
}

int main(void)
{
	display_init(640, 480, 150);
	
	display_background(BLACK);
	display_test1();
	delay_ms(5000);
	display_background(BLUE);
	display_test2();
	delay_ms(10000);
	display_test3();

	return 0;
}
