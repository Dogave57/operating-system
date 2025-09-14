#include "kernel.h"
#include "stdlib.h"
#include "memory.h"
#include "sys_service.h"
static uint32_t syscall_list[32] = {0};
void sys_getsyscall_list(uint32_t* plist);
int sys_printf(const char* fmt, ...){
	sys_getsyscall_list(syscall_list);
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_PRINTF]));
	return 0;
}
int sys_print(const char* msg){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_PRINT]));
	while (1){};
	return 0;
}
int sys_putchar(char ch){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_PUTCHAR]));
	while (1){};
	return 0;
}
int sys_kmalloc(unsigned int size){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_KMALLOC]));
	while (1){};
	return 0;
}
int sys_kfree(void* p){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_KFREE]));
	while (1){};
	return 0;
}
struct file* sys_openfile(unsigned int drive, const char* filename){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_OPENFILE]));
	while(1){};
	return (struct file*)0x0;
}
int sys_createfile(unsigned int drive, const char* filename){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_CREATEFILE]));
	while (1){};
	return 0;
}
int sys_deletefile(struct file* pfile){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_DELETEFILE]));
	while (1){};
	return 0;
}
int sys_readfile(struct file* pfile, unsigned char* pbuf){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_READFILE]));
	while (1){};
	return 0;
}
int sys_writefile(struct file* pfile, unsigned char* pbuf, unsigned int size){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_WRITEFILE]));
	while (1){};
	return 0;
}
int sys_renamefile(struct file* pfile, const char* newname){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_RENAMEFILE]));
	while (1){};
	return 0;
}
unsigned int sys_getfilesize(struct file* pfile){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_GETFILESIZE]));
	while (1){};
	return 0;
}
int sys_closefile(struct file* pfile){
	if (!syscall_list[0])
		sys_getsyscall_list(syscall_list);
	__asm__ volatile("jmp *%0" :: "r"(syscall_list[SYSV_CLOSEFILE]));
	while (1){};
	return 0;
}
