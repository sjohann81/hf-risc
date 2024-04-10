#include <hf-risc.h>
#include "vga_drv.h"


struct ball_s {
	unsigned int ballx, bally;
	unsigned int last_ballx, last_bally;
	int dx, dy;
};


void init_display()
{
	display_background(BLACK);
	display_frectangle(100, 100, 25, 35, GREEN);
	display_fcircle(250, 50, 25, YELLOW);
	display_ftriangle(30, 170, 75, 195, 50, 150, RED);
	display_frectangle(230, 180, 25, 10, CYAN);
}

void init_ball(struct ball_s *ball, int x, int y, int dx, int dy)
{
	ball->ballx = x;
	ball->bally = y;
	ball->last_ballx = ball->ballx;
	ball->last_bally = ball->bally;
	ball->dx = dx;
	ball->dy = dy;
}

void init_input()
{
	/* configure GPIOB pins 8 .. 12 as inputs */
	GPIOB->DDR &= ~(MASK_P8 | MASK_P9 | MASK_P10 | MASK_P11 | MASK_P12);
}



void test_limits(char *limits, struct ball_s *ball)
{
	unsigned int ballx, bally;
	
	ballx = ball->ballx;
	bally = ball->bally;
	
	display_pixel(ball->last_ballx, ball->last_bally, BLACK);
	limits[0] = display_getpixel(ballx-2, bally-2);
	limits[1] = display_getpixel(ballx, bally-1);
	limits[2] = display_getpixel(ballx+2, bally-2);
	limits[3] = display_getpixel(ballx+1, bally);
	limits[4] = display_getpixel(ballx+2, bally+2);
	limits[5] = display_getpixel(ballx, bally+1);
	limits[6] = display_getpixel(ballx-2, bally+2);
	limits[7] = display_getpixel(ballx-1, bally);
	limits[8] = display_getpixel(ballx, bally);
	display_pixel(ball->last_ballx, ball->last_bally, WHITE);
}

char test_collision(char *limits, struct ball_s *ball)
{
	char hit = 0;
	int i;
	
	if ((ball->ballx < VGA_WIDTH-1) && (ball->ballx > 0) && (ball->bally < VGA_HEIGHT-1) && (ball->bally > 0)) {
		for (i = 0; i < 9; i++) {
			if (limits[i]) {
				hit = 1;
				break;
			}
		}
		
		if (limits[0]) {
			ball->dx = -ball->dx;
			ball->dy = -ball->dy;
		}
		if (limits[2]) {
			ball->dx = -ball->dx;
			ball->dy = -ball->dy;
		}
		if (limits[4]) {
			ball->dx = -ball->dx;
			ball->dy = -ball->dy;
		}
		if (limits[6]) {
			ball->dx = -ball->dx;
			ball->dy = -ball->dy;
		}
		
		if (limits[1]) ball->dy = -ball->dy;
		if (limits[3]) ball->dx = -ball->dx;
		if (limits[5]) ball->dy = -ball->dy;
		if (limits[7]) ball->dx = -ball->dx;
	} else {
		if ((ball->ballx + ball->dx > VGA_WIDTH-1) || (ball->ballx + ball->dx < 1))
			ball->dx = -ball->dx;
		if ((ball->bally + ball->dy > VGA_HEIGHT-1) || (ball->bally + ball->dy < 1))
			ball->dy = -ball->dy;
	}
	
	return hit;
}

void update_ball(struct ball_s *ball)
{
	display_pixel(ball->last_ballx, ball->last_bally, BLACK);
	display_pixel(ball->ballx, ball->bally, WHITE);

	ball->last_ballx = ball->ballx;
	ball->last_bally = ball->bally;
	ball->ballx = ball->ballx + ball->dx;
	ball->bally = ball->bally + ball->dy;
}

void get_input()
{
	if (GPIOB->IN & MASK_P10)
		display_frectangle(30, (VGA_HEIGHT/2)-10, 5, 20, YELLOW);
	else
		display_frectangle(30, (VGA_HEIGHT/2)-10, 5, 20, BLACK);
		
	if (GPIOB->IN & MASK_P11)
		display_frectangle(VGA_WIDTH-31, (VGA_HEIGHT/2)-10, 5, 20, YELLOW);
	else
		display_frectangle(VGA_WIDTH-31, (VGA_HEIGHT/2)-10, 5, 20, BLACK);
		
	if (GPIOB->IN & MASK_P9)
		display_frectangle((VGA_WIDTH/2)-10, 0, 20, 5, YELLOW);
	else
		display_frectangle((VGA_WIDTH/2)-10, 0, 20, 5, BLACK);
		
	if (GPIOB->IN & MASK_P12)
		display_frectangle((VGA_WIDTH/2)-10, VGA_HEIGHT-6, 20, 5, YELLOW);
	else
		display_frectangle((VGA_WIDTH/2)-10, VGA_HEIGHT-6, 20, 5, BLACK);
}

int main(void)
{
	struct ball_s ball1, ball2;
	struct ball_s *pball1 = &ball1;
	struct ball_s *pball2 = &ball2;
	char limits[9];
	
	init_display();
	init_ball(pball1, 150, 105, 1, 1);
	init_ball(pball2, 25, 17, 1, -1);
	init_input();

	while (1) {
		test_limits(limits, pball1);
		test_collision(limits, pball1);
		test_limits(limits, pball2);
		test_collision(limits, pball2);
		delay_ms(10);
		update_ball(pball1);
		update_ball(pball2);
		get_input();
	}

	return 0;
}
