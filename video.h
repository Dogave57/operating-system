#ifndef _VIDEO
#define _VIDEO
#include "vga.h"
enum objType{
	OBJ_INVALID,
	OBJ_RECT,
};
struct vector2{
	int x, y;
};
struct object{
	enum objType type;
	struct vector2 position;
	struct vector2 size;
	unsigned char color;
	struct object* blink;
	struct object* flink;
	struct vector2 oldposition;
	struct vector2 oldsize;
	unsigned char oldcolor;
};
extern struct object* objectlist;
extern struct object* lastobject;
extern unsigned int objectcnt;
extern unsigned char* vga_buffer;
extern unsigned int vga_width;
extern unsigned int vga_height;
extern enum videoMode video_mode;
void print(const char* str);
void putchar(char ch);
void puthex(unsigned char hex, unsigned char lower);
void clear();
void vga_set_bg(enum vgaColor color);
int vga_init_objects(void);
int vga_deinit_objects(void);
struct object* vga_add_object(struct vector2 position, struct vector2 size, enum objType type);
int vga_remove_object(struct object* pobject);
int vga_init(void);
int vga_write_char(unsigned int offset, unsigned char ch);
int vga_write_pixel(int x, int y, enum vgaColor color);
void vga_set_mode(enum videoMode mode);
void vga_set_color(unsigned char fg, unsigned char bg);
#endif
