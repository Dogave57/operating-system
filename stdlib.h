#ifndef _STDLIB
#define _STDLIB
#include <stddef.h>
int itoa(int num, char* buf, size_t bufmax);
int atoi(char* buf);
size_t strlen(char* str);
int memcmp(void* buf1, void* buf2, size_t size);
int memcpy(void* dest, void* src, size_t size);
int memset(void* buf, unsigned char val, size_t size);
char toUpper(char ch);
char toLower(char ch);
void* kmalloc(size_t size);
int kfree(void* data);
#endif
