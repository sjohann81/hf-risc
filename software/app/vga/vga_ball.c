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
	char collision = 0;
	int i;
	
	if ((ball->ballx < VGA_WIDTH-1) && (ball->ballx > 0) && (ball->bally < VGA_HEIGHT-1) && (ball->bally > 0)){
		for (i = 0; i < 9; i++) {
			if (limits[i]) {
				collision = 1;
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
	
	return collision;
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


int main(void)
{
	struct ball_s ball;
	struct ball_s *pball = &ball;
	char limits[9];
	
	init_display();
	init_ball(pball, 150, 105, 1, 1);

	while (1) {
		test_limits(limits, pball);
		test_collision(limits, pball);
		
		delay_ms(10);

		update_ball(pball);
	}

	return 0;
}
