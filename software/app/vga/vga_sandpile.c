#include <hf-risc.h>
#include "vga_drv.h"

#define WIDTH	100
#define HEIGHT	100

int initsp(char matrix[WIDTH][HEIGHT]){
	int i, j;

	for (i = 0; i < WIDTH; i++)
		for (j = 0; j < HEIGHT; j++)
			matrix[i][j] = 0;

	return 0;
}

int put_grain(char matrix[WIDTH][HEIGHT]){
	int i, j;

	i = WIDTH >> 1;
	j = HEIGHT >> 1;
	matrix[i][j]++;

	return 0;
}

int showsp(char matrix[WIDTH][HEIGHT]){
	int i, j;

	for (i = 0; i < WIDTH; i++)
		for (j = 0; j < HEIGHT; j++)
			display_quad(i << 1, j << 1, matrix[i][j]);

	return 0;
}

int avalanche(char matrix[WIDTH][HEIGHT]){
	int i, j;
	char aux_matrix[WIDTH][HEIGHT];

	for (i = 0; i < WIDTH; i++)
		for (j = 0; j < HEIGHT; j++)
			aux_matrix[i][j] = matrix[i][j];

	for (i = 0; i < WIDTH; i++) {
		for (j = 0; j < HEIGHT; j++){
			if (matrix[i][j] >= 4){
				if ((j - 1) >= 0) aux_matrix[i][j-1]++;
				if ((j + 1) < HEIGHT) aux_matrix[i][j+1]++;
				if ((i - 1) >= 0) aux_matrix[i-1][j]++;
				if ((i + 1) < WIDTH) aux_matrix[i+1][j]++;
				aux_matrix[i][j] -= 4;
			}
		}
	}

	for (i = 0; i < WIDTH; i++)
		for (j = 0; j < HEIGHT; j++)
			matrix[i][j] = aux_matrix[i][j];

	return 0;
}

void draw_sandpile(void){
	char m[WIDTH][HEIGHT];
	int (*next_step)(char matrix[WIDTH][HEIGHT]);	// ponteiro para funcoes
	int step, done;

	step = 0; done = 0;
	while (!done){					// state machine do algoritmo
		switch (step++){
		case 0:
			next_step = initsp;
			break;
		case 1:
			next_step = showsp;
			break;
		case 2:
			next_step = put_grain;
			break;
		case 3:
			next_step = avalanche;
			break;
		default:
			step = 1;
			break;
		}
		done = next_step(m);			// executa o proximo passo
	}
}

int main(void)
{
	display_background(BLACK);
	draw_sandpile();

	return 0;
}
