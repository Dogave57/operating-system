#include "kernel.h"
#include "stdlib.h"
#include "memory.h"
#include "sys_service.h"
__attribute__((noreturn)) int sys_printf(const char* fmt, ...){
	__asm__ volatile("mov $1, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))int sys_print(const char* msg){
	__asm__ volatile("mov $2, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))int sys_putchar(char ch){
	__asm__ volatile("mov $3, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))int sys_kmalloc(unsigned int size){
	__asm__ volatile("mov $12, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))int sys_kfree(void* p){
	__asm__ volatile("mov $13, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))struct file* sys_openfile(unsigned int drive, const char* filename){
	__asm__ volatile("mov $4, %ecx");
	__asm__ volatile("int $0x90");
	while(1){};
}
__attribute__((noreturn))int sys_createfile(unsigned int drive, const char* filename){
	__asm__ volatile("mov $5, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))int sys_deletefile(struct file* pfile){
	__asm__ volatile("mov $6, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))int sys_readfile(struct file* pfile, unsigned char* pbuf){
	__asm__ volatile("mov $7, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))int sys_writefile(struct file* pfile, unsigned char* pbuf, unsigned int size){
	__asm__ volatile("mov $8, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))int sys_renamefile(struct file* pfile, const char* newname){
	__asm__ volatile("mov $9, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))unsigned int sys_getfilesize(struct file* pfile){
	__asm__ volatile("mov $10, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
__attribute__((noreturn))int sys_closefile(struct file* pfile){
	__asm__ volatile("mov $11, %ecx");
	__asm__ volatile("int $0x90");
	while (1){};
}
