#ifndef _MEMORY
#define _MEMORY
#include <stdint.h>
struct memorymap_entry{
	uint32_t baselow;
	uint32_t basehigh;
	uint32_t sizelow;
	uint32_t sizehigh;
	uint32_t type;
	uint32_t acpi;
};
unsigned int getAvalibleMemory(void);
#endif
