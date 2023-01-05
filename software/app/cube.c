#include <hf-risc.h>
#include <math.h>
#include "display/display_drv.h"

struct cube_s {
	float node[8][3];
	int edge[12][2];
};


void init_cube(struct cube_s *cube)
{
	const float node[8][3] = {
		{-1.0, -1.0, -1.0}, {-1.0, -1.0, 1.0},
		{-1.0, 1.0, -1.0}, {-1.0, 1.0, 1.0},
		{1.0, -1.0, -1.0}, {1.0, -1.0, 1.0},
		{1.0, 1.0, -1.0}, {1.0, 1.0, 1.0}
	};

	const int edge[12][2] = {
		{0, 1}, {1, 3}, {3, 2}, {2, 0},
		{4, 5}, {5, 7}, {7, 6}, {6, 4},
		{0, 4}, {1, 5}, {2, 6}, {3, 7}
	};
	
	memcpy(cube->node, node, sizeof(node));
	memcpy(cube->edge, edge, sizeof(edge));
}

void scale_cube(struct cube_s *cube, float s)
{
	for (int i = 0; i < 8; i++) {
		cube->node[i][0] *= s;
		cube->node[i][1] *= s;
		cube->node[i][2] *= s;
	}
}

void rotate_cube(struct cube_s *cube, float angle_x, float angle_y)
{
	float sin_x = sin(angle_x);
	float cos_x = cos(angle_x);

	float sin_y = sin(angle_y);
	float cos_y = cos(angle_y);

	for (int i = 0; i < 8; i++) {
		float x = cube->node[i][0];
		float y = cube->node[i][1];
		float z = cube->node[i][2];

		cube->node[i][0] = x * cos_x - z * sin_x;
		cube->node[i][2] = z * cos_x + x * sin_x;
		z = cube->node[i][2];
		cube->node[i][1] = y * cos_y - z * sin_y;
		cube->node[i][2] = z * cos_y + y * sin_y;
	}
}

void draw_cube(struct cube_s *cube, int x_off, int y_off, uint16_t color)
{
	float xy1[2], xy2[2];
	int e0, e1;

	for (int i = 0; i < 12; i++) {
		e0 = cube->edge[i][0];
		e1 = cube->edge[i][1];
		xy1[0] = cube->node[e0][0] + x_off;
		xy1[1] = cube->node[e0][1] + y_off;
		xy2[0] = cube->node[e1][0] + x_off;
		xy2[1] = cube->node[e1][1] + y_off;
		display_line((int)xy1[0], (int)xy1[1], (int)xy2[0], (int)xy2[1], color);
	}
}

int main(void)
{
	struct cube_s cube1, cube2;
	
	display_init(640, 480, 150);
	
	display_background(BLACK);
	
	init_cube(&cube1);
	scale_cube(&cube1, 50.0);
	rotate_cube(&cube1, PI / 4.0, atan(sqrt(2.0)));
	
	init_cube(&cube2);
	scale_cube(&cube2, 70.0);
	rotate_cube(&cube2, PI / 4.0, atan(sqrt(2.0)));
	
	for (int i = 0; i < 1000; i++) {
		rotate_cube(&cube1, PI / 180.0, PI / 150.0);
		rotate_cube(&cube2, PI / 180.0, PI / 120.0);
		draw_cube(&cube1, 200, 100, BLUE);
		draw_cube(&cube2, 400, 300, OLIVE);
		display_blit();
		draw_cube(&cube1, 200, 100, BLACK);
		draw_cube(&cube2, 400, 300, BLACK);
	}
	
	return 0;
}
