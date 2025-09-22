#include <stdarg.h>
#include <stdint.h>
#include "cursor.h"
#include "stdlib.h"
#include "filesystem.h"
#include "panic.h"
#include "bootloader.h"
#include "timer.h"
#include "vga.h"
#include "video.h"
unsigned int vgaIndex = 0;
unsigned char* vga_buffer = (unsigned char*)0xA0000;
unsigned int vga_width = 320;
unsigned int vga_height = 200;
unsigned int vga_attrib = 0x07;
unsigned int vga_fg = VGA_COLOR_WHITE;
unsigned int vga_bg = VGA_COLOR_BLACK;
unsigned char* font_buffer = (unsigned char*)0x0;
void print(const char* str){
	if (!str)
		str = "Invalid string\n";
	for (unsigned int i = 0;str[i];i++){
		putchar(str[i]);
	}
	return;
}
void putchar(char ch){
	switch (ch){
		case '\n':
			vgaIndex+=(320)/8;
			vgaIndex-=(vgaIndex%(320/8));
		break;
		case '\r':
		break;
			vgaIndex-=vgaIndex%vga_width*2;
		break;
		case '\b':
			  vgaIndex--;
			  unsigned int line = vgaIndex/(320/8);
			  vga_write_char((vgaIndex*8)+(320*8*line), ' ');
		break;
		default:{
			unsigned int line = vgaIndex/(320/8);
			vga_write_char((vgaIndex*8)+(320*8*line), ch);	
			vgaIndex++;
			break;
		}
	}
	if (vgaIndex/(320/8)>(200/8)){
		clear();
		return;
	}
	cursor_setpos(vgaIndex/2);
	return;
}
void puthex(unsigned char hex, unsigned char lower){
	if (hex>16)
		return;
	if (hex<10){
		putchar('0'+hex);
		return;
	}
	if (lower){
		putchar('a'+hex-10);
		return;
	}
	putchar('A'+hex-10);
	return;
}
void clear(void){
	enum vgaColor colors[] = {VGA_COLOR_LIGHT_RED, VGA_COLOR_LIGHT_GREEN, VGA_COLOR_LIGHT_BLUE};
	for (unsigned int i = 0;i<vga_width*vga_height;i++){
		vga_buffer[i] = colors[random(0, sizeof(colors)/sizeof(colors[0]))];
	}
	vgaIndex = 0;
	cursor_setpos(0);
	return;
}
int vga_write_char(unsigned int offset, unsigned char ch){
	if (!vga_buffer){
		if (vga_init()!=0)
			return -1;
	}
	unsigned char* pdataoff = font_buffer+(((8*8)/8)*ch);
	for (int x = 0;x<8;x++){
		for (int y = 0;y<8;y++){
		unsigned int vga_offset = offset+(y*vga_width)+x;
		unsigned int font_byte = (((y*8)+x)/8);
		unsigned int font_bit = 8-(((y*8)+x)%8);
		unsigned int isfg = pdataoff[font_byte]&(1<<font_bit);
		if (isfg)
			vga_buffer[vga_offset] = vga_fg;
		else
			vga_buffer[vga_offset] = vga_bg;
		}
	}
	return 0;
}
int vga_write_pixel(unsigned int x, unsigned int y, enum vgaColor color){
	unsigned int vga_off = (y*vga_width)+x;
	vga_buffer[vga_off] = color;
	return 0;
}
int vga_draw_rect(struct vector2 pos, struct vector2 size, enum vgaColor color){
	struct vector2 coords = {0};
	for (coords.y = pos.y; coords.y<pos.y+size.y;coords.y++){
		for (coords.x = pos.x; coords.x<pos.x+size.x;coords.x++){
			vga_write_pixel(coords.x, coords.y, color);
		}
	}	
	return 0;
}
int vga_init(void){
	unsigned int bootdrive = getbootdrive();
	struct file* pfile = openfile(bootdrive, "fonts/font.font");
	if (!pfile){
		panic("failed to open fonts file\n");
		return -1;
	}
	unsigned int filesize = getfilesize(pfile);
	font_buffer = (unsigned char*)kmalloc(filesize);
	if (!font_buffer){
		panic("failed to allocate memory for font buffer\n");
		return -1;
	}
	if (readfile(pfile, font_buffer)!=0){
		panic("failed to read font file\n");
		return -1;
	}
	closefile(pfile);
	for (unsigned int i = 0;i<=9;i++){
		vga_write_char(i*4, '0'+i);
	}
	return 0;
}
void vga_set_color(unsigned char fg, unsigned char bg){
	vga_fg = fg;
	vga_bg = bg;
	return;
}
