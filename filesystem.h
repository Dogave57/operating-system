#ifndef _FILESYSTEM
#define _FILESYSTEM
#include <stdint.h>
#include "kernel.h"
#define FS_RESERVED_SECTORS 128
#define EPICFS_SIGNATURE 0x43495045 //EPIC
struct epic_fshdr{
	unsigned int signature;
	unsigned int bytes_per_cluster;
	unsigned int fat_size;
	unsigned int data_off;
}__attribute__((packed));
int read_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int wordsPersector);
int write_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int wordsPersector);
int drive_getinfo(unsigned int drive, uint16_t* info);
int epic_get_fsinfo(unsigned int drive, struct epic_fshdr* pinfo);
struct highlow_64 drive_getsectors(unsigned int drive);
unsigned int getbootdrive(void);
#endif
