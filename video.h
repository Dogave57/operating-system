#ifndef _VIDEO
#define _VIDEO
#include "vga.h"
extern unsigned char* vga_buffer;
extern unsigned int vga_width;
extern unsigned int vga_height;
void print(const char* str);
void putchar(char ch);
void puthex(unsigned char hex, unsigned char lower);
void clear();
int vga_init(void);
int vga_write_char(unsigned int offset, unsigned char ch);
int vga_write_pixel(unsigned int x, unsigned int y, enum vgaColor color);
void vga_set_color(unsigned char fg, unsigned char bg);
#endif
