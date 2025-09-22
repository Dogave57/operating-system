#include <stdint.h>
#include "bootloader.h"
#include "panic.h"
#include "stdlib.h"
#include "memory.h"
#define align_up(val, align)((val+align-1) & ~(align-1))
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
void* kmalloc(size_t size){
	if (!heap_data){
		if (heap_init()!=0)
			return NULL;
	}
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
	if (heap_data->freeblock_cnt!=0){
		for (unsigned int i = 0;i<heap_data->freeblock_cnt;i++){
			struct heap_block* pblock = (struct heap_block*)(*(heap_data->freelist-heap_data->freeblock_cnt+i));
			if (!pblock||pblock->inuse!=0||pblock->datasize<size){
			continue;
			}
			unsigned int dt = pblock->datasize-size;
			if (dt&&dt>sizeof(struct heap_block)){
			pblock->datasize = size;
			struct heap_block* newblock = (struct heap_block*)((unsigned char*)(pblock+1)+pblock->datasize);
			pblock->flink->blink = newblock;
			pblock->flink = newblock;
			newblock->blink = newblock;
			newblock->inuse = 0;
			newblock->datasize = dt-sizeof(struct heap_block);
			}else{
			heap_data->freeblock_cnt--;
			}
			pblock->inuse = 1;
			return (void*)(pblock+1);
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

