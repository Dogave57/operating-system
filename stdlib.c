#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "memory.h"
#include "panic.h"
#include "libsys.h"
#define align_up(val, align)((val+align-1) & ~(align-1))
int itoa(int num, char* buf, size_t bufmax){
	if (!buf||bufmax<1)
		return -1;
	if (!num){
		buf[0] = '0';
		return 0;
	}
	if (num<0){
		num*=-1;
		buf[0]='-';
		buf++;
		bufmax--;
	}
	unsigned int digitcnt = 0;
	int tmp = num;
	for (;tmp;tmp/=10){
		digitcnt++;
	}
	for (unsigned int i = 0;i<digitcnt;i++){
		buf[digitcnt-i-1] = '0'+(num%10);
		num/=10;
	}
	return 0;
}
int atoi(char* buf){
	if (!buf)
		return -1;
	int res = 0;
	for (unsigned int i = 0;buf[i];i++){
		if (buf[i]<'0'||buf[i]>'9')
			continue;
		unsigned int digit = buf[i]-'0';
		if (((int64_t)res*10+digit)>2146483647||((int64_t)res*10+digit)<-2147483647)
			break;
		res = res*10+digit;
	}
	return res;
}
size_t strlen(char* str){
	if (!str)
		return 0;
	size_t len = 0;
	for (;str[len];len++);
	return len;
}
int memcmp(void* buf1, void* buf2, size_t size){
	if (!buf1||!buf2||size<1)
		return 1;
	unsigned char isaligned = size%4;
	for (size_t i = 0;i<size;){
		/*if (isaligned){
			if (*(unsigned int*)((unsigned char*)buf1+i)!=*(unsigned int*)((unsigned char*)buf2+i))
				return 1;
			i+=4;
			continue;
		}*/
		if (*((unsigned char*)buf1+i)!=*((unsigned char*)buf2+i))
			return 1;
		i++;
	}
	return 0;
}
int strcmp(char* str1, char* str2){
	if (!str1||!str2)
		return 1;
	for (unsigned int i = 0;;i++){
		if (str1[i]!=str2[i])
			return 1;
		if (!str1[i])
			break;
	}
	return 0;
}
int memcpy(void* dest, const void* src, size_t size){
	if (!dest||!src||size<1)
		return -1;
	unsigned char isaligned = !(size%4);
	for (size_t i = 0;i<size;){
		if (isaligned){
			*(unsigned int*)((unsigned char*)dest+i) = *(unsigned int*)((unsigned char*)src+i);
			i+=4;
			continue;
		}
		*((unsigned char*)dest+i)=*((unsigned char*)src+i);
		i++;
	}
	return 0;
}
int strcpy(char* dest, const char* src){
	if (!dest||!src)
		return -1;
	for (unsigned int i = 0;;i++){
		dest[i] = src[i];
		if (!src[i])
			break;
	}
	return 0;
}
int memset(void* buf, unsigned char val, size_t size){
	if (!buf)
		return -1;
	for (size_t i = 0;i<size;i++){
		*((unsigned char*)buf+i) = val;
	}
	return 0;
}
char toUpper(char ch){
	static const char upperTable[]={
		['1'] = '!',
		['2'] = '@',
		['3'] = '#',
		['4'] = '$',
		['5'] = '%',
		['6'] = '^',
		['7'] = '&',
		['8'] = '*',
		['9'] = '(',
		['0'] = ')',
		['`'] = '~',
		['-'] = '_',
		['='] = '+',
		['['] = '{',
		[']'] = '}',
		[';'] = ':',
		['\''] = '"',
		[','] = '<',
		['.'] = '>',
		['/'] = '?',
		['\\'] = '|',
	};
	if (ch<'a'||ch>'z')
		return upperTable[ch];
	return (ch-'a'+'A');
}
char toLower(char ch){
	static const char lowerTable[]={
		['!'] = '1',
		['@'] = '2',
		['#'] = '3',
		['$'] = '4',
		['%'] = '5',
		['^'] = '6',
		['&'] = '7',
		['*'] = '8',
		['('] = '9',
		[')'] = '0',
		['~'] = '`',
		['_'] = '-',
		['+'] = '=',
		['{'] = '[',
		['}'] = ']',
		[':'] = ';',
		['"'] = '\'',
		['<'] = ',',
		['>'] = '.',
		['?'] = '/',
		['|'] = '\\'
	};
	if (ch<'A'||ch>'Z')
		return lowerTable[ch];
	return (ch-'A'+'a');
}
void printf(const char* fmt, ...){
	if (!fmt)
		fmt = "Invalid string\n";
	va_list args = 0;
	va_start(args, fmt);
	for (unsigned int i = 0;fmt[i];i++){
		if (fmt[i]!='%'){
			sys_putchar(fmt[i]);
			continue;
		}
		switch (fmt[i+1]){
			case 'd':{
				int num = va_arg(args,  int);
				char buf[64]={0};
				itoa(num, buf, sizeof(buf));
				sys_print(buf);
				i++;
				break;
			}
			case 'c':{
				char ch = (char)va_arg(args, int);
				sys_putchar(ch);
				i++;
				break;
			}
			case 's':{
				char* str = (char*)va_arg(args, char*);
				sys_print(str);
				i++;
				break;
			}
			case 'p':{
				uint32_t ptr = (uint32_t)va_arg(args, uint32_t);
				for (int i = 7;i>-1;i--){
					uint8_t hex = ptr>>(4*i);
					hex&=0xF;
					sys_puthex(hex, 1);
				}
				i++;
				break;
			}
			case 'x':{
				uint32_t ptr = (uint32_t)va_arg(args, uint32_t);
				for (int i = 7;i>-1;i--){
					uint8_t hex = ptr>>(4*i);
					hex&=0xF;
					sys_puthex(hex, 1);
				}
				i++;
				break;
			}
			case 'X':{
				uint32_t ptr = (uint32_t)va_arg(args, uint32_t);
				for (int i = 7;i>-1;i--){
					uint8_t hex = ptr>>(4*i);
					hex&=0xF;
					sys_puthex(hex, 0);
				}
				i++;
				break;
			}
		}
	}
	return;
}
