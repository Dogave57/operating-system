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
struct object* objectlist = (struct object*)0x0;
struct object* lastobject = (struct object*)0x0;
unsigned int objectcnt = 0;
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
		//vga_buffer[i] = VGA_COLOR_BLACK;
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
int vga_write_pixel(int x, int y, enum vgaColor color){
	int vga_off = (y*vga_width)+x;
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
int vga_init_objects(void){
	if (objectlist){
		if (vga_deinit_objects()!=0)
			return -1;
	}
	objectcnt = 0;
	return 0;	
}
int vga_deinit_objects(void){
	if (!objectlist)
		return 0;
	struct object* currentobject = objectlist;
	while (currentobject){
		struct object* flink = currentobject->flink;
		kfree((void*)currentobject);
		currentobject = flink;
	}
	objectlist = (struct object*)0x0;
	objectcnt = 0;
	return 0;
}
struct object* vga_add_object(struct vector2 position, struct vector2 size, enum objType type){
	struct object* newobject = (struct object*)kmalloc(sizeof(struct object));
	if (!newobject)
		return (struct object*)0x0;
	memset((void*)newobject, 0, sizeof(struct object));
	newobject->position = position;
	newobject->size = size;
	newobject->type = type;
	if (!objectlist){
		objectlist = newobject;
		return newobject;
	}
	if (!lastobject){
		lastobject = newobject;
		return newobject;
	}
	lastobject->flink = newobject;
	newobject->blink = lastobject;
	lastobject = newobject;
	return newobject;
}
int vga_remove_object(struct object* pobject){
	if (!pobject)
		return -1;
	if (pobject->blink)
		pobject->blink->flink = pobject->flink;
	if (pobject->flink)
		pobject->flink->blink = pobject->blink;
	return 0;	
}
int vga_render_objects(void){
	struct object* currentobject = objectlist;
	while (currentobject){
		if (currentobject->oldposition.x==currentobject->position.x&&currentobject->oldposition.x==currentobject->position.x&&currentobject->oldsize.x==currentobject->size.x&&currentobject->oldsize.y==currentobject->size.y&&currentobject->color==currentobject->oldcolor)
			continue;
		switch (currentobject->type){
		case OBJ_RECT:	
		vga_draw_rect(currentobject->oldposition, currentobject->oldsize, VGA_COLOR_BLACK);
		vga_draw_rect(currentobject->position, currentobject->size, currentobject->color);
		currentobject->oldposition = currentobject->position;
		currentobject->oldsize = currentobject->oldsize;
		currentobject->oldcolor = currentobject->color;
		break;	
		}	
		currentobject = currentobject->flink;
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
