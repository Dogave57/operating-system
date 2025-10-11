#ifndef _STDLIB
#define _STDLIB
#include <stddef.h>
int itoa(int num, char* buf, size_t bufmax);
int atoi(char* buf);
size_t strlen(char* str);
int memcmp(void* buf1, void* buf2, size_t size);
int strcmp(char* str1, char* str2);
int memcpy(void* dest, const void* src, size_t size);
int strcpy(char* dest, const char* src);
int memset(void* buf, unsigned char val, size_t size);
char toUpper(char ch);
char toLower(char ch);
void* kmalloc(unsigned int size);
int kfree(void* data);
int printf(const char* fmt, ...);
int scan(char* buf, unsigned int bufmax, unsigned char end);
#endif
