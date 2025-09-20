#ifndef _LIBSYS
#define _LIBSYS
#include "filesystem.h"
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
#endif
