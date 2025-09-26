#ifndef _LIBSYS
#define _LIBSYS
#include "filesystem.h"
#include "input.h"
#include "video.h"
#include "vga.h"
void sys_putchar(char ch);
void sys_print(char* msg);
void sys_puthex(unsigned char hex, unsigned int isUpper);
void* sys_kmalloc(unsigned int size);
void sys_kfree(void* p);
struct file* sys_openfile(unsigned int drive, char* filename);
int sys_closefile(struct file* pfile);
int sys_deletefile(struct file* pfile);
unsigned int sys_getfilesize(struct file* pfile);
int sys_writefile(struct file* pfile, unsigned char* buf, unsigned int size);
int sys_readfile(struct file* pfile, unsigned char* buf);
int sys_getbootdrive(void);
void sys_clear(void);
char sys_getchar(void);
void sys_set_console_color(enum vgaColor fg, enum vgaColor bg);
int sys_loadelf(unsigned int drive, char* filename);
int sys_getfileinfo(struct file* pfile, struct fileinfo* pinfo);
int sys_createfile(unsigned int drive, char* filename, enum fileType type);
int sys_getfilelist(unsigned int drive, struct file* pdir, struct fileinfo** pplist, unsigned int* plist_entries);
int sys_renamefile(struct file* pfile, char* newname);
unsigned int sys_get_time_ms(void);
int sys_writepixel(int x, int y, enum vgaColor color);
unsigned int sys_random(unsigned int min, unsigned int max);
int sys_draw_rect(struct vector2 pos, struct vector2 size, enum vgaColor color);
void sys_sleep(unsigned int ms);
void sys_panic(char* reason);
int sys_init_objects(void);
int sys_deinit_objects(void);
struct object_rect* sys_add_rect(struct vector2 position, struct vector2 size, enum frameType frame);
struct object_text* sys_add_text(struct vector2 position, char* text, enum frameType frame);
int sys_remove_object(struct object* pobject);
int sys_render_objects(void);
unsigned int sys_keypressed(unsigned int key);
void sys_set_bg(enum vgaColor color);
enum vgaColor sys_readpixel(int x, int y);
struct object_vertices* sys_add_vertices(struct vector2* pvertices, unsigned int vertices_size, enum frameType frame);
int sys_playsound(unsigned int frequency);
int sys_stopsound(void);
int sys_beep(void);
#endif
