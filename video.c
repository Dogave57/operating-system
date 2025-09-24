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
enum vgaColor vga_bg = VGA_COLOR_BLACK;
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
		//vga_buffer[i] = colors[random(0, sizeof(colors)/sizeof(colors[0]))];
		vga_buffer[i] = vga_bg;
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
enum vgaColor vga_read_pixel(int x, int y){
	int vga_off = (y*vga_width)+x;
	return vga_buffer[vga_off];
}
int vga_draw_rect(struct vector2 pos, struct vector2 size, enum vgaColor color){
	struct vector2 coords = {0};
	for (coords.y = pos.y; coords.y<pos.y+size.y;coords.y++){
		for (coords.x = pos.x; coords.x<pos.x+size.x;coords.x++){
			if (coords.x>=vga_width||coords.x<=0||coords.y<=0||coords.y>=vga_height)
				continue;
			vga_write_pixel(coords.x, coords.y, color);
		}
	}	
	return 0;
}
void vga_set_bg(enum vgaColor color){
	vga_bg = color;
	return;
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
struct object_rect* vga_add_rect(struct vector2 position, struct vector2 size){
	struct object_rect* newobject = (struct object_rect*)kmalloc(sizeof(struct object_rect));
	if (!newobject)
		return (struct object_rect*)0x0;
	memset((void*)newobject, 0, sizeof(struct object_rect));
	newobject->flink = (struct object*)0x0;
	newobject->position = position;
	newobject->size = size;
	newobject->type = OBJ_RECT;
	if (!objectlist){
		objectlist = (struct object*)newobject;
		lastobject = (struct object*)newobject;
		return newobject;
	}
	if (!lastobject){
		lastobject = (struct object*)newobject;
		return newobject;
	}
	lastobject->flink = (struct object*)newobject;
	newobject->blink = (struct object*)lastobject;
	lastobject = (struct object*)newobject;
	return newobject;
}
struct object_text* vga_add_text(struct vector2 position, char* text){
	if (!text)
		return (struct object_text*)0x0;
	struct object_text* textobj = (struct object_text*)kmalloc(sizeof(struct object_text));
	if (!text)
		return (struct object_text*)0x0;
	memset((void*)textobj, 0, sizeof(struct object_text));
	textobj->flink = (struct object*)0x0;
	textobj->position = position;
	textobj->text = text;
	textobj->type = OBJ_TEXT;
	if (!objectlist){
		objectlist = (struct object*)textobj;
		lastobject = (struct object*)textobj;
		return textobj;
	}
	if (!lastobject){
		lastobject = (struct object*)textobj;
		return textobj;
	}
	lastobject->flink = (struct object*)textobj;
	textobj->blink = lastobject;
	lastobject = (struct object*)textobj;
	return textobj;
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
		switch (currentobject->type){
		case OBJ_RECT:	 
		struct object_rect* rect = (struct object_rect*)currentobject;
	//	if (rect->position.x==rect->oldposition.x&&rect->position.y==rect->oldposition.x&&rect->size.x==rect->oldsize.y&&rect->size.y==rect->oldsize.y&&rect->color==rect->oldcolor)
	//		break;
		struct vector2 coords = {0};
		for (coords.y = rect->oldposition.y;coords.y<rect->oldposition.y+rect->oldsize.y;coords.y++){
			for (coords.x = rect->oldposition.x;coords.x<rect->oldposition.x+rect->oldsize.x;coords.x++){
				if (coords.x<rect->position.x+rect->size.x&&coords.x>rect->position.x&&coords.y<rect->position.y+rect->size.y&&coords.y>rect->position.y)
					continue;
				if (vga_read_pixel(coords.x, coords.y)!=rect->oldcolor)
					continue;
				vga_write_pixel(coords.x, coords.y, vga_bg);
			}
		}	
		vga_draw_rect(rect->position, rect->size, rect->color);
		rect->oldposition = rect->position;
		rect->oldsize.x = rect->size.x;
		rect->oldsize.y = rect->size.y;
		rect->oldcolor = rect->color;
		break;	
		case OBJ_TEXT:
		struct object_text* text = (struct object_text*)currentobject;
	//	if (text->position.x==text->oldposition.x&&text->position.y==text->oldposition.y&&text->color==text->oldcolor||!text->text)
	//		break;
		unsigned int index = 0;
		for (unsigned int i = 0;;i++){
			unsigned char ch = text->text[i];
			if (!ch)
				break;
			if (ch=='\n'){
				index+=320/8;
				index-=index%(320/8);
				continue;
			}
			unsigned int line = index/(320/8);
			unsigned int offset = (index*8)+(line*320*8);
			vga_write_char((text->position.y*vga_width)+text->position.x+offset, ch);
			index++;
		}
		text->oldposition.x = text->position.x;
		text->oldposition.y = text->position.y;
		text->oldcolor = text->color;
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
	vga_bg = VGA_COLOR_BLACK;
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
