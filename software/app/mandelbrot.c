#include <hf-risc.h>
#include "display/display_drv.h"

int mandelbrot(float C_re, float C_im, int max_count)
{
	int count = 0;
	float Z_re = 0;
	float Z_im = 0;
	float magnitude = 0;
	float max_magnitude = 1000000;

	// Calculate Z=Z^2+C for given C
	while ((magnitude < max_magnitude) && (count++ < max_count)) {
		float N_re = Z_re * Z_re - Z_im * Z_im + C_re;
		float N_im = Z_re * Z_im + Z_im * Z_re + C_im;
		Z_re = N_re;
		Z_im = N_im;
		magnitude = Z_re * Z_re + Z_im * Z_im;
	}

	// Return number of iterations 
	if (count >= max_count)
		return 0;
	else
		return count;
}

void draw_mandelbrot(int width, int height)
{
	float center_re = -0.5;
	float center_im = 0.0;
	float radius = 1.0;
	uint16_t color;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float C_re = center_re - radius + (2 * radius * x) / width;
			float C_im = center_im - radius + (2 * radius * y) / height;

			color = 4 * mandelbrot(C_re, C_im, 64);

			display_pixel(x, y, RGB(color, color, color));
		}
		display_blit();
	}
}

int main(void)
{
	int width = 256, height = 256;

	display_init(width, height, 200);
	display_background(BLACK);
	draw_mandelbrot(width, height);

	return 0;
}
