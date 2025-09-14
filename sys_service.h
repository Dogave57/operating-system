#ifndef _SYS_SERVICE
#define _SYS_SERVICE
#define SYSV_PRINTF 1
#define SYSV_PRINT 2
#define SYSV_PUTCHAR 3
#define SYSV_OPENFILE 4
#define SYSV_CREATEFILE 5
#define SYSV_DELETEFILE 6
#define SYSV_READFILE 7
#define SYSV_WRITEFILE 8
#define SYSV_RENAMEFILE 9
#define SYSV_GETFILESIZE 10
#define SYSV_CLOSEFILE 11
#define SYSV_KMALLOC 12
#define SYSV_KFREE 13
#include "stdlib.h"
#include "video.h"
#include "kernel.h"
#include "filesystem.h"
int sys_printf(const char* fmt, ...);
int sys_print(const char* msg);
int sys_putchar(char ch);
int sys_kmalloc(unsigned int size);
int sys_kfree(void* p);
struct file* sys_openfile(unsigned int drive, const char* filename);
int sys_createfile(unsigned int drive, const char* filename);
int sys_deletefile(struct file* pfile);
int sys_readfile(struct file* pfile, unsigned char* pbuf);
int sys_writefile(struct file* pfile, unsigned char* pbuf, unsigned int size);
int sys_renamefile(struct file* pfile, const char* newname);
unsigned int sys_getfilesize(struct file* pfile);
int sys_closefile(struct file* pfile);
#endif
