#ifndef _MEMORY
#define _MEMORY
#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
extern struct heap_metadata* heap_data;
struct memorymap_entry{
	uint32_t baselow;
	uint32_t basehigh;
	uint32_t sizelow;
	uint32_t sizehigh;
	uint32_t type;
	uint32_t acpi;
};
struct heap_block{
	uint32_t datasize;
	unsigned int inuse;
	struct heap_block* flink;
	struct heap_block* blink;
};
struct heap_metadata{
	unsigned int heap_reserved;
	unsigned int heapused;
	unsigned int freeblock_cnt;
	struct heap_block** freelist;
	struct heap_block* firstblock;
	struct heap_block* currentblock;
};
unsigned int getAvalibleMemory(void);
uint64_t getInstalledMemory(void);
int heap_init(void);
#endif
