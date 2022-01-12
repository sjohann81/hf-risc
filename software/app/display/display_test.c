#include <hf-risc.h>
#include "display_drv.h"

void display_test1()
{
	char buf[30];
		
	display_pixel(10, 10, YELLOW);
	display_line(0, 0, display_p->width, display_p->height, GREEN);
	display_line(display_p->width, 0, 0, display_p->height, GREEN);
	display_rectangle(0, 0, 240, 240, LIME);
	display_frectangle(20, 10, 100, 20, RED);
	display_circle(95,170,15, WHITE);
	display_fcircle(25, 65, 20, BLUE);
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
	int i, j, k = 32;
	
	for (j = 10; k < 127; j += 8) {
		for (i = 1; i < display_p->width - 1 - FONT_X && k < 127; i += FONT_SPACE, k++) {
			display_char(k, i, j, 1, YELLOW);
		}
	}
	
	display_blit();
}

int main(void)
{
	display_init(640, 480, 150);
	
	display_background(BLACK);
	display_test1();
	delay_ms(5000);
	display_background(BLUE);
	display_test2();
	delay_ms(5000);

	return 0;
}
