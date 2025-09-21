#ifndef _VIDEO
#define _VIDEO
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_BUFFER 0xB8000
void print(const char* str);
void putchar(char ch);
void puthex(unsigned char hex, unsigned char lower);
void clear();
void vga_set_color(unsigned char fg, unsigned char bg);
#endif
