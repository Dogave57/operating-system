#include <stdarg.h>
#include <stdint.h>
#include "cursor.h"
#include "stdlib.h"
#include "video.h"
unsigned int vgaIndex = 0;
void print(const char* str){
	if (!str)
		str = "Invalid string\n";
	for (unsigned int i = 0;str[i];i++){
		putchar(str[i]);
	}
	return;
}
void printf(const char* fmt, ...){
	if (!fmt)
		fmt = "Invalid string\n";
	va_list args = 0;
	va_start(args, fmt);
	for (unsigned int i = 0;fmt[i];i++){
		if (fmt[i]!='%'){
			putchar(fmt[i]);
			continue;
		}
		switch (fmt[i+1]){
			case 'd':{
				int num = va_arg(args,  int);
				char buf[64]={0};
				itoa(num, buf, sizeof(buf));
				print(buf);
				i++;
				break;
			}
			case 'c':{
				char ch = (char)va_arg(args, int);
				putchar(ch);
				i++;
				break;
			}
			case 's':{
				char* str = (char*)va_arg(args, char*);
				print(str);
				i++;
				break;
			}
			case 'p':{
				uint32_t ptr = (uint32_t)va_arg(args, uint32_t);
				for (int i = 7;i>-1;i--){
					uint8_t hex = ptr>>(4*i);
					hex&=0xF;
					puthex(hex, 1);
				}
				i++;
				break;
			}
			case 'x':{
				uint32_t ptr = (uint32_t)va_arg(args, uint32_t);
				for (int i = 7;i>-1;i--){
					uint8_t hex = ptr>>(4*i);
					hex&=0xF;
					puthex(hex, 1);
				}
				i++;
				break;
			}
			case 'X':{
				uint32_t ptr = (uint32_t)va_arg(args, uint32_t);
				for (int i = 7;i>-1;i--){
					uint8_t hex = ptr>>(4*i);
					hex&=0xF;
					puthex(hex, 0);
				}
				i++;
				break;
			}
		}
	}
	return;
}
void putchar(char ch){
	if (vgaIndex>=VGA_WIDTH*VGA_HEIGHT*2)
		clear();
	switch (ch){
		case '\n':	
			vgaIndex+=VGA_WIDTH*2;
			vgaIndex-=(vgaIndex%(VGA_WIDTH*2));
		break;
		case '\r':
			vgaIndex-=(vgaIndex%(VGA_WIDTH*2));
		break;
		case '\b':
			vgaIndex-=2;
			*((volatile unsigned char*)0xB8000+vgaIndex) = ' ';
		break;
		default:
			*((volatile unsigned char*)0xB8000+vgaIndex) = ch;
			*((volatile unsigned char*)0xB8000+vgaIndex+1) = 0x07;
			vgaIndex+=2;
		break;
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
		*((volatile unsigned char*)0xB8000+i) = ' ';
	}
	vgaIndex = 0;
	cursor_setpos(0);
	return;
}
