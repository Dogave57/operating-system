#include <stdint.h>
#include "bootloader.h"
#include "panic.h"
#include "stdlib.h"
#include "memory.h"
#define align_up(val, align)((val+align-1) & ~(align-1))
struct heap_metadata heap_metadata = {0};
unsigned int avalibleMemory = 0;
uint64_t installedMemory = 0;
unsigned int getAvalibleMemory(void){
	if (avalibleMemory!=0)
		return avalibleMemory;
	struct bootloader_args* blargs = (struct bootloader_args*)0x2000;
	for (unsigned int i = 0;i<blargs->memorymap_entries;i++){
		struct memorymap_entry entry = blargs->memorymap[i];
		if (entry.type!=1)
			continue;
		if ((uint64_t)avalibleMemory+entry.sizelow>2147483647){
			avalibleMemory = 2147483647;
			break;
		}
		avalibleMemory+=entry.sizelow;
	}
	return avalibleMemory;
}
uint64_t getInstalledMemory(void){
	if (installedMemory!=0)
		return installedMemory;
	struct bootloader_args* blargs = (struct bootloader_args*)0x2000;
	for (unsigned int i = 0;i<blargs->memorymap_entries;i++){
		struct memorymap_entry entry = blargs->memorymap[i];
		installedMemory += *(uint64_t*)&entry.sizelow;
	}
	return installedMemory;
}
int heap_init(void){
	memset((void*)&heap_metadata, 0, sizeof(struct heap_metadata));
	heap_metadata.heapstart = (unsigned char*)2000000;
	unsigned int installedMem = (unsigned int)getInstalledMemory();
	unsigned int heap_reserved = installedMem-(unsigned int)heap_metadata.heapstart;
	heap_metadata.pfreelist = (unsigned int*)(heap_metadata.heapstart+heap_reserved-4);
	return 0;
}
void* kmalloc(unsigned int size){
	size = align_up(size, 4);	
	if (!heap_metadata.currentblock){
		heap_metadata.currentblock = (struct heap_block*)heap_metadata.heapstart;
		heap_metadata.currentblock->datasize = size;
		heap_metadata.currentblock->inuse = 1;
		return (void*)(heap_metadata.currentblock+1);
	}
	if (heap_metadata.freeblockcnt){
		for (unsigned int i = 0;i<heap_metadata.freeblockcnt;i++){
			struct heap_block* pblock = (struct heap_block*)(*(heap_metadata.pfreelist-i));
			if (!pblock||pblock->inuse!=0||pblock->datasize<size)
				continue;
			unsigned int dt = pblock->datasize-size;
			if (dt&&dt>sizeof(struct heap_block)){
				struct heap_block* pdtblock = (struct heap_block*)(pblock->data+size);
				pdtblock->datasize = dt-sizeof(struct heap_block);
				pdtblock->inuse = 0;
				*(heap_metadata.pfreelist-i) = (unsigned int)pdtblock;
				if (pdtblock>heap_metadata.currentblock)
					heap_metadata.currentblock = pdtblock;
			}
			else{
				*(heap_metadata.pfreelist-i) = (unsigned int)0;
				heap_metadata.freeblockcnt--;
			}
			pblock->inuse = 1;
			pblock->datasize = size;
			return (void*)(pblock+1);
		}
	}
	if (heap_metadata.currentblock){
		struct heap_block* pblock = (struct heap_block*)(heap_metadata.currentblock->data+heap_metadata.currentblock->datasize);
		pblock->datasize = size;
		pblock->inuse = 1;
		heap_metadata.currentblock = pblock;
		return (void*)(pblock+1);	
	}
	return NULL;
}
int kfree(void* data){
	if (!data)
		return -1;
	struct heap_block* pblock = (struct heap_block*)data;
	pblock--;
	pblock->inuse = 0;
	*(heap_metadata.pfreelist-heap_metadata.freeblockcnt) = (unsigned int)pblock;
	heap_metadata.freeblockcnt++;
	return 0;
}

