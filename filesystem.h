#ifndef _FILESYSTEM
#define _FILESYSTEM
#include <stdint.h>
#include "kernel.h"
struct fs_link{
	char filename[32];
	unsigned int linksize;
	unsigned int index;
	unsigned int parent;
	unsigned int left;
	unsigned int right;
	char data[1];
};
int read_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer);
int write_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer);
int drive_getinfo(unsigned int drive, uint16_t* info);
struct highlow_64 drive_getsectors(unsigned int drive);
unsigned int getbootdrive(void);
#endif
