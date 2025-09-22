#include <stdarg.h>
#include <stdint.h>
#include "cursor.h"
#include "stdlib.h"
#include "video.h"
unsigned int vgaIndex = 0;
unsigned int vga_attrib = 0x07;
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
			vgaIndex+=VGA_WIDTH*2;
			vgaIndex-=(vgaIndex%(VGA_WIDTH*2));
		break;
		case '\r':
			vgaIndex-=vgaIndex%VGA_WIDTH*2;
		break;
		case '\b':
			  vgaIndex-=2;
			  ((unsigned char*)VGA_BUFFER)[vgaIndex] = ' ';
		break;
		default:{
			((unsigned char*)VGA_BUFFER)[vgaIndex] = ch;
			((unsigned char*)VGA_BUFFER)[vgaIndex+1] = vga_attrib;
			vgaIndex+=2;
			break;
		}
	}
	if (vgaIndex>=(VGA_WIDTH*VGA_HEIGHT*2)){
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
	for (unsigned int i = 0;i<VGA_WIDTH*VGA_HEIGHT*2;i+=2){
		((volatile unsigned char*)VGA_BUFFER)[i] = ' ';
	}
	vgaIndex = 0;
	cursor_setpos(0);
	return;
}
void vga_set_color(unsigned char fg, unsigned char bg){
	vga_attrib = (bg << 4)|(fg&0xF);
	for (unsigned int i = 0;i<VGA_WIDTH*VGA_HEIGHT*2;i+=2){
		*((volatile unsigned char*)VGA_BUFFER+i+1) = vga_attrib;
	}
	return;
}
