#include <stdint.h>
#include "bootloader.h"
#include "memory.h"
unsigned int getAvalibleMemory(void){
	struct bootloader_args* blargs = (struct bootloader_args*)0x2000;
	unsigned int avalible = 0;
	for (unsigned int i = 0;i<blargs->memorymap_entries;i++){
		struct memorymap_entry entry = blargs->memorymap[i];
		if (entry.type!=1)
			continue;
		if ((uint64_t)avalible+entry.sizelow>2147483647){
			avalible = 2147483647;
			break;
		}
		avalible+=entry.sizelow;
	}
	return avalible;
}
