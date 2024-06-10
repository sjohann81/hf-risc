#define VGA_WIDTH	300
#define VGA_HEIGHT	218

#define BLACK		0
#define BLUE		1
#define GREEN		2
#define CYAN		3
#define RED		4
#define MAGENTA		5
#define YELLOW		6
#define WHITE		7
#define GRAY		8
#define LBLUE		9
#define LGREEN		10
#define LCYAN		11
#define LRED		12
#define LMAGENTA	13
#define LYELLOW		14
#define LWHITE		15

#define FONT_X		8
#define FONT_Y		8

void display_quad(uint16_t x, uint16_t y, char color);
void display_pixel(uint16_t x, uint16_t y, char color);
char display_getpixel(uint16_t x, uint16_t y);
void display_line(uint16_t x0,uint16_t y0,uint16_t x1, uint16_t y1, uint16_t color);
void display_hline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);
void display_vline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);
void display_rectangle(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color);
void display_frectangle(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color);
void display_background(uint16_t color);
void display_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void display_fcircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void display_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void display_ftriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void display_char(uint8_t ascii, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor);
void display_print(char *string, uint16_t x0, uint16_t y0, uint16_t size, uint16_t fgcolor);

#define _swap_int16_t(a, b)	\
{				\
	int16_t t = a;		\
	a = b;			\
	b = t;			\
}


