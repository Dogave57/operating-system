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
struct frame frame_bg = {0};
struct frame frame_middle = {0};
struct frame frame_fg = {0};
unsigned int objectcnt = 0;
unsigned char* vga_buffer = (unsigned char*)0xA0000;
unsigned char* vga_backbuffer = (unsigned char*)0x0;
struct vga_tile* vga_tilelist = (struct vga_tile*)0x0;
unsigned int current_tile = 0;
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
			vgaIndex+=(vga_width)/8;
			vgaIndex-=(vgaIndex%(vga_width/8));
		break;
		case '\r':
		break;
			vgaIndex-=vgaIndex%vga_width*2;
		break;
		case '\b':
			  vgaIndex--;
			  unsigned int line = vgaIndex/(vga_width/8);
			  vga_write_char((vgaIndex*8)+(vga_width*8*line), ' ', vga_fg, vga_bg);
		break;
		default:{
			unsigned int line = vgaIndex/(vga_width/8);
			vga_write_char((vgaIndex*8)+(vga_width*8*line), ch, vga_fg, vga_bg);	
			vgaIndex++;
			break;
		}
	}
	if (vgaIndex*64>((vga_width*vga_height))){
		clear();	
		vgaIndex = 0;
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
		vga_buffer[i] = vga_bg;
	}	
	vgaIndex = 0;
	cursor_setpos(0);
	return;
}
int vga_write_char(unsigned int offset, unsigned char ch, enum vgaColor fg, enum vgaColor bg){
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
			vga_write_pixel(vga_offset, fg);
		else
			vga_write_pixel(vga_offset, bg);
		}
	}
	return 0;
}
int vga_write_coord(unsigned int x, unsigned int y, enum vgaColor color){
	return vga_write_pixel((y*vga_width)+x, color);
}
int vga_write_pixel(unsigned int pixel, enum vgaColor color){
	while (!(inb(0x3DA)&8)){};
	vga_buffer[pixel] = color;
	while (inb(0x3DA)&8){};
	return 0;
	unsigned int planesize = (vga_width*vga_height)/8;
	unsigned int x = pixel%vga_width;
	unsigned int vga_byte = pixel/8;
	unsigned int vga_bit = x%8;
	for (unsigned int i = 0;i<4;i++){
		unsigned char* plane = vga_buffer+(i*planesize);
			
	}
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
			vga_write_coord(coords.x, coords.y, color);
		}
	}	
	return 0;
}
void vga_set_bg(enum vgaColor color){
	vga_bg = color;
	return;
}
int vga_init_objects(void){
	memset((void*)vga_backbuffer, 255, vga_width*vga_height);
	current_tile = 0;
	return 0;
}
int vga_deinit_frame(struct frame* pframe){
	if (!pframe)
		return -1;
	struct object* currentobject = pframe->objlist;
	while (currentobject){
		struct object* flink = currentobject->flink;
		kfree((void*)currentobject);
		currentobject = flink;
	}
	pframe->objlist = (struct object*)0x0;
	return 0;
}
int vga_deinit_objects(void){
	vga_deinit_frame(&frame_bg);
	vga_deinit_frame(&frame_middle);
	vga_deinit_frame(&frame_fg);
	current_tile = 0;
	return 0;
}
int vga_add_object(struct frame* pframe, struct object* pobject){
	if (!pframe||!pobject)
		return -1;
	if (!pframe->objlist){
		pframe->objlist = pobject;
		pframe->lastobj = pobject;
		return 0;
	}
	if (!pframe->lastobj){
		pframe->lastobj = pobject;
		return 0;
	}
	pframe->lastobj->flink = pobject;
	pobject->blink = pframe->lastobj;
	pframe->lastobj = pobject;
	return 0;
}
struct object_rect* vga_add_rect(struct vector2 position, struct vector2 size, enum frameType frame){
	struct object_rect* newobject = (struct object_rect*)kmalloc(sizeof(struct object_rect));
	if (!newobject)
		return (struct object_rect*)0x0;
	memset((void*)newobject, 0, sizeof(struct object_rect));
	newobject->flink = (struct object*)0x0;
	newobject->position = position;
	newobject->size = size;
	newobject->ssize = sizeof(struct object_rect);
	newobject->type = OBJ_RECT;
	objectcnt++;
	struct frame* frame_map[] = {
	[FRAME_BG] = &frame_bg,
	[FRAME_MIDDLE] = &frame_middle,
	[FRAME_FG] = &frame_fg,
	};
	struct frame* frame_mapping = frame_map[frame];
	if (vga_add_object(frame_mapping, (struct object*)newobject)!=0)
		return (struct object_rect*)0x0;
	return newobject;
}
struct object_text* vga_add_text(struct vector2 position, char* text, enum frameType frame){
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
	textobj->ssize = sizeof(struct object_text);
	objectcnt++;
	struct frame* frame_map[] = {
	[FRAME_BG] = &frame_bg,
	[FRAME_MIDDLE] = &frame_middle,
	[FRAME_FG] = &frame_fg,
	};
	struct frame* frame_mapping = frame_map[frame];
	if (vga_add_object(frame_mapping, (struct object*)textobj)!=0)
		return (struct object_text*)0x0;
	return textobj;
}
struct object_vertices* vga_add_vertices(struct vector2* pvertices, unsigned int vertices_size,enum frameType frame){
	if (!pvertices)
		return (struct object_vertices*)0x0;	
	struct object_vertices* newobj = (struct object_vertices*)kmalloc(sizeof(struct object_vertices));
	if (!newobj){
		return (struct object_vertices*)0x0;
	}
	memset((void*)newobj, 0, sizeof(struct object_vertices));
	newobj->type = OBJ_VERTICES;
	newobj->ssize = sizeof(struct object_vertices);	
	newobj->pvertices = pvertices;
	newobj->vertices_size = vertices_size;
	struct frame* frame_map[]={
	[FRAME_BG] = &frame_bg,
	[FRAME_MIDDLE] = &frame_middle,
	[FRAME_FG] = &frame_fg,
	};
	struct frame* frame_mapping = frame_map[frame];
	if (vga_add_object(frame_mapping, (struct object*)newobj)!=0)
		return (struct object_vertices*)0x0;
	return newobj;
}
int vga_remove_object(struct object* pobject){
	if (!pobject)
		return -1;
	if (pobject->blink)
		pobject->blink->flink = pobject->flink;
	if (pobject->flink)
		pobject->flink->blink = pobject->blink;
	objectcnt--;
	return 0;	
}
int vga_render_frame(struct frame* pframe){
	if (!pframe)
		return -1;
	struct object* currentobject = pframe->objlist;
	while (currentobject){
		switch (currentobject->type){
		case OBJ_RECT:{
		struct object_rect* rect = (struct object_rect*)currentobject;
		struct vector2 coords = {0};
/*		for (coords.y = rect->oldposition.y;coords.y<rect->oldposition.y+rect->oldsize.y;coords.y++){
			for (coords.x = rect->oldposition.x;coords.x<rect->oldposition.x+rect->oldsize.x;coords.x++){
				if (coords.x<rect->position.x+rect->size.x&&coords.x>rect->position.x&&coords.y<rect->position.y+rect->size.y&&coords.y>rect->position.y)
					continue;
				if (vga_read_pixel(coords.x, coords.y)!=rect->oldcolor)
					continue;
				vga_write_coord(coords.x, coords.y, vga_bg);
			}
		}*/	
		vga_draw_rect(rect->position, rect->size, rect->color);
		rect->oldposition = rect->position;
		rect->oldsize.x = rect->size.x;
		rect->oldsize.y = rect->size.y;
		rect->oldcolor = rect->color;
		break;	
		}
		case OBJ_TEXT:{
		struct object_text* text = (struct object_text*)currentobject;
		unsigned int index = 0;
		for (unsigned int i = 0;;i++){
			unsigned char ch = text->text[i];
			if (!ch)
				break;
			if (ch=='\n'){
				index+=vga_width/8;
				index-=index%(vga_width/8);
				continue;
			}
			unsigned int line = index/(vga_width/8);
			unsigned int offset = (index*8)+(line*vga_width*8);
			vga_write_char((text->position.y*vga_width)+text->position.x+offset, ch, text->color, vga_bg);
			index++;
		}
		text->oldposition.x = text->position.x;
		text->oldposition.y = text->position.y;
		text->oldcolor = text->color;
		break;
		}
		case OBJ_VERTICES:{
		struct object_vertices* vertices = (struct object_vertices*)currentobject;
		struct vector2* plastpoint = (struct vector2*)0x0;
		unsigned int vertexcnt = vertices->vertices_size/sizeof(struct vector2);
		for (unsigned int i = 1;i<vertexcnt;i++){
			struct vector2* pvertex = vertices->pvertices+i;
			struct vector2* plastvertex = vertices->pvertices+i-1;
			struct vector2 dt = {plastvertex->x-pvertex->x, plastvertex->y-pvertex->y};
			if (dt.x<0)
				dt.x*=-1;
			if (dt.y<0)
				dt.y*=-1;
			vga_write_char((pvertex->y*vga_width)+pvertex->x, 'a', vga_fg, vga_bg);
			vga_write_char((plastvertex->y*vga_width)+plastvertex->x, 'a', vga_fg, vga_bg);
			struct vector2 coords = {0};
			for (;;){
				if (coords.x<=dt.x){
				coords.x++;
				}	
				if (coords.y<=dt.y){
				coords.y++;
				}
				if (coords.x>dt.x&&coords.y>dt.y){
				break;
				}
				struct vector2* xvec = pvertex;
				if (pvertex->x>plastvertex->x)
					xvec = plastvertex;
				struct vector2* yvec = pvertex;
				if (pvertex->y>plastvertex->y)
					yvec = plastvertex;
				vga_write_coord(xvec->x+coords.x,yvec->y+coords.y, VGA_COLOR_BLACK);
			}
		}	
		break;
		}
		}	
		currentobject = currentobject->flink;
	}
	return 0;
}
int vga_render_objects(void){
	unsigned char* oldbuffer = vga_buffer;
	vga_buffer = vga_backbuffer;
	clear();
	vga_render_frame(&frame_bg);
	vga_render_frame(&frame_middle);
	vga_render_frame(&frame_fg);
	vga_buffer = oldbuffer;
	memcpy((void*)vga_buffer, (void*)vga_backbuffer, vga_width*vga_height);
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
		vga_write_char(i*4, '0'+i, vga_fg, vga_bg);
	}
	vga_backbuffer = (unsigned char*)kmalloc(vga_width*vga_height);
	if (!vga_backbuffer){
		panic("failed to allocate memory for back buffer\n");
		return -1;
	}		
	memset((void*)vga_backbuffer, 255, vga_width*vga_height);
	vga_tilelist = (struct vga_tile*)kmalloc((vga_width*vga_height)/VGA_TILE_SIZE);
	if (!vga_tilelist){
		panic("failed to allocate memory for tile list\n");
		return -1;
	}
	return 0;
}
void vga_set_color(unsigned char fg, unsigned char bg){
	vga_fg = fg;
	vga_bg = bg;
	return;
}
