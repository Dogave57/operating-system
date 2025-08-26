#include <stdint.h>
#include "bootloader.h"
#include "memory.h"
struct heap_metadata* heap_data = (struct heap_metadata*)NULL;
struct heap_block* largestFreeBlock = (struct heap_block*)NULL;
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
	struct heap_block* firstblock = (struct heap_block*)NULL;
	heap_data = (struct heap_metadata*)2000000;
	firstblock = (struct heap_block*)(heap_data+1);
	heap_data->firstblock = (struct heap_block*)(firstblock);
	heap_data->heap_reserved = getInstalledMemory()-(unsigned int)heap_data;
	heap_data->freeblock_cnt = 0;
	heap_data->freelist = (struct heap_block**)((unsigned char*)heap_data->firstblock+heap_data->heap_reserved);
	return 0;
}
