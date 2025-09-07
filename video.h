#ifndef _VIDEO
#define _VIDEO
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_BUFFER 0xB8000
enum vgaColor{
	VGA_COLOR_BLACK,
	VGA_COLOR_BLUE,
	VGA_COLOR_GREEN,
	VGA_COLOR_CYAN,
	VGA_COLOR_RED,
	VGA_COLOR_MAGENTA,
	VGA_COLOR_BROWN,
	VGA_COLOR_WHITE,
	VGA_COLOR_GRAY,
	VGA_COLOR_LIGHT_BLUE,
	VGA_COLOR_LIGHT_GREEN,
	VGA_COLOR_LIGHT_CYAN,
	VGA_COLOR_LIGHT_RED,
	VGA_COLOR_LIGHT_MAGENTA,
	VGA_COLOR_YELLOW,
	VGA_COLOR_BRIGHT_WHITE,
};
void print(const char* str);
void printf(const char* fmt, ...);
void putchar(char ch);
void puthex(unsigned char hex, unsigned char lower);
void clear();
void vga_set_color(unsigned char fg, unsigned char bg);
#endif
