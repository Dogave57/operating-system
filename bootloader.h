
#ifndef _BOOTLOADER
#define _BOOTLOADER
#include <stdint.h>
struct bootloader_args{
	int bootdrive;
	char vendorid[13];
	char productname[51];//49 bytes + 2 for alignment
	struct memorymap_entry* memorymap;
	uint32_t memorymap_entries;
}__attribute__((packed));
#endif
