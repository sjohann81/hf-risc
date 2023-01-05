/* RGB565 display hardware*/
#define DISPLAY_RAM			(*(volatile uint32_t *)(0xe0400000))	// 0xe0400000 - 0xe07fffff (4MB)
#define DISPLAY_STATUS			(*(volatile uint32_t *)(0xe0e00010))	// bit: 31 - 4 (reserved), 3 - update (blit), 2 - deinitialize, 1 - initialize, 0 - configured
#define DISPLAY_WIDTH			(*(volatile uint32_t *)(0xe0e00020))
#define DISPLAY_HEIGHT			(*(volatile uint32_t *)(0xe0e00030))
#define DISPLAY_SCALE			(*(volatile uint32_t *)(0xe0e00040))

#define DISPLAY_CONFIGURED		(1 << 0)
#define DISPLAY_INITIALIZE		(1 << 1)
#define DISPLAY_DEINITIALIZE		(1 << 2)
#define DISPLAY_UPDATE			(1 << 3)

/* RGB565 display driver and library*/
struct display_s {
	uint16_t *ptr;
	uint16_t width;
	uint16_t height;
	uint16_t scale;
};

extern struct display_s *display_p;

/* R:00001 G:000001 B:00001 */
#define RGB(red, green, blue)	((uint16_t)(((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3)))

/* https://www.rapidtables.com/web/color/RGB_Color.html */
#define BLACK		RGB(0x00, 0x00, 0x00)
#define WHITE		RGB(0xff, 0xff, 0xff)
#define RED		RGB(0xff, 0x00, 0x00)
#define LIME		RGB(0x00, 0xff, 0x00)
#define BLUE		RGB(0x00, 0x00, 0xff)
#define YELLOW		RGB(0xff, 0xff, 0x00)
#define CYAN		RGB(0x00, 0xff, 0xff)
#define MAGENTA		RGB(0xff, 0x00, 0xff)
#define SILVER		RGB(0xc0, 0xc0, 0xc0)
#define GRAY		RGB(0x80, 0x80, 0x80)
#define MAROON		RGB(0x80, 0x00, 0x00)
#define OLIVE		RGB(0x80, 0x80, 0x00)
#define GREEN		RGB(0x00, 0x80, 0x00)
#define PURPLE		RGB(0x80, 0x00, 0x80)
#define TEAL		RGB(0x00, 0x80, 0x80)
#define NAVY		RGB(0x00, 0x00, 0x80)

#define FONT_X		8
#define FONT_Y		8

void display_init(uint16_t width, uint16_t height, uint16_t scale);
void display_blit(void);
void display_pixel(uint16_t x0, uint16_t y0, uint16_t color);
void display_line(uint16_t x0,uint16_t y0,uint16_t x1, uint16_t y1, uint16_t color);
void display_hline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);
void display_vline(uint16_t x0, uint16_t y0, uint16_t length, uint16_t color);
void display_rectangle(uint16_t x0, uint16_t y0, uint16_t length, uint16_t width, uint16_t color);
void display_frectangle(uint16_t x0, uint16_t y0, uint16_t length, uint16_t width, uint16_t color);
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


