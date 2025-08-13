#ifndef _FILESYSTEM
#define _FILESYSTEM
#include <stdint.h>
#include "kernel.h"
#define FS_SIGNATURE 0x56525458//VRTX in little endian
#define FS_RESERVED_SECTORS 128
struct filesystem_info{
	unsigned int signature;
	unsigned char fsname[16];
}__attribute__((packed));
int read_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int bytespersector);
int write_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int bytespersector);
int drive_getinfo(unsigned int drive, uint16_t* info);
struct highlow_64 drive_getsectors(unsigned int drive);
unsigned int getbootdrive(void);
#endif
