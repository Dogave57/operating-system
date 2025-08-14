#include <stddef.h>
#include <stdint.h>
#include "memory.h"
#include "bootloader.h"
#include "panic.h"
#include "kernel.h"
#include "video.h"
#include "stdlib.h"
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
int memcpy(void* dest, const void* src, size_t size){
	if (!dest||!src||size<1)
		return -1;
	unsigned char isaligned = size%4;
	for (size_t i = 0;i<size;){
		if (isaligned){
			*((unsigned int*)dest+i)=*((unsigned int*)src+i);
			i+=4;
			continue;
		} 
		*((unsigned char*)dest+i)=*((unsigned char*)src+i);
		i++;
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
void* kmalloc(size_t size){
	if (size<1)
		return NULL;
	size = align_up(size, 4);
	if (size>heap_data->heap_reserved){
		return NULL;
	}
	heap_data->heapused+=size;
	if (heap_data->heapused>heap_data->heap_reserved){
		panic("out of memory");	
		return NULL;
	}
	if (!heap_data->currentblock){
		heap_data->currentblock = heap_data->firstblock;
		heap_data->currentblock->datasize = size;
		heap_data->currentblock->inuse = 1;
		heap_data->firstblock = heap_data->currentblock;
		return (void*)(heap_data->currentblock+1);
	}
	if (heap_data->freeblock_cnt){
		struct heap_block* currentlink = (struct heap_block*)heap_data->firstblock;	
		for (int i = heap_data->freeblock_cnt;i>-1;i--){
			currentlink = *(heap_data->freelist-i);
			if (currentlink->inuse!=0||currentlink->datasize<size){
				continue;
			}
			uint32_t sizedt = currentlink->datasize-size;
			currentlink->datasize = size;
			if (sizedt){
				struct heap_block* newblock = (struct heap_block*)((unsigned char*)currentlink+sizeof(struct heap_block)+currentlink->datasize);
				newblock->flink = currentlink->flink;	
				currentlink->flink->blink = newblock;
				currentlink->flink = newblock;
				newblock->datasize = sizedt;
				newblock->inuse = 0;
			}else
				heap_data->freeblock_cnt--;
			currentlink->inuse = 1;
			return (void*)(currentlink+1);	
		}
	}
	if (heap_data->currentblock){
		uint32_t offset = heap_data->currentblock->datasize+sizeof(struct heap_block);
		struct heap_block* newblock = (struct heap_block*)(((unsigned char*)heap_data->currentblock)+offset);
		newblock->datasize = size;
		newblock->inuse = 1;
		newblock->blink = heap_data->currentblock;
		heap_data->currentblock->flink = newblock;
		heap_data->currentblock = newblock;
		return (void*)(newblock+1);
	}
	return NULL;
}
int kfree(void* data){
	if (!data)
		return -1;
	struct heap_block* heapblock = (struct heap_block*)((unsigned char*)data-sizeof(struct heap_block));
	heap_data->heapused -= heapblock->datasize;
	*(heap_data->freelist-heap_data->freeblock_cnt) = heapblock;
	heapblock->inuse = 0;
	heap_data->freeblock_cnt++;
	return 0;
}
