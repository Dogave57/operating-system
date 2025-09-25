#ifndef _VIDEO
#define _VIDEO
#include "vga.h"
enum objType{
	OBJ_INVALID,
	OBJ_RECT,
	OBJ_TEXT,
};
enum frameType{
	FRAME_INVALID,
	FRAME_BG,
	FRAME_MIDDLE,
	FRAME_FG,
};
struct vector2{
	int x, y;
};
struct object{
	enum objType type;
	unsigned int ssize;
	struct object* blink;
	struct object* flink;
};
struct frame{
	enum frameType type;
	struct object* objlist;
	struct object* lastobj;
};
struct object_rect{
	enum objType type;
	unsigned int ssize;
	struct object* blink;
	struct object* flink;
	struct vector2 position;
	struct vector2 size;
	unsigned char color;
	struct vector2 oldposition;
	struct vector2 oldsize;
	unsigned char oldcolor;
};
struct object_text{
	enum objType type;
	unsigned int ssize;
	struct object* blink;
	struct object* flink;
	struct vector2 position;
	unsigned char color;
	struct vector2 oldposition;
	unsigned char oldcolor;
	char* text;
};
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
int vga_deinit_frame(struct frame* pframe);
int vga_deinit_objects(void);
struct object_rect* vga_add_rect(struct vector2 position, struct vector2 size, enum frameType frame);
struct object_text* vga_add_text(struct vector2 position, char* text, enum frameType frame);
int vga_remove_object(struct object* pobject);
int vga_init(void);
int vga_write_char(unsigned int offset, unsigned char ch, enum vgaColor fg, enum vgaColor bg);
int vga_write_pixel(int x, int y, enum vgaColor color);
enum vgaColor vga_read_pixel(int x, int y);
void vga_set_mode(enum videoMode mode);
void vga_set_color(unsigned char fg, unsigned char bg);
#endif
