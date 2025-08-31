#ifndef _FILESYSTEM
#define _FILESYSTEM
#include <stdint.h>
#include "kernel.h"
#define FS_RESERVED_SECTORS 128
#define EPICFS_SIGNATURE (unsigned int)0x43495045 //EPIC
#define EPICFS_EOC (unsigned int)0xFFFE
#define EPICFS_FC (unsigned int)0x0
enum fsType{
	FS_NONE,
	FS_EPIC,
};
enum clusterType{
	CLUSTER_INVALID,
	CLUSTER_FILE,
	CLUSTER_FILEDATA,
};
enum fileType{
	FILE_INVALID,
	FILE_REGULAR,
	FILE_DIR,
};
struct epic_fshdr{
	unsigned int signature;
	unsigned int bytes_per_cluster;
	unsigned int fat_size;
	unsigned int data_off;
}__attribute__((packed));
struct epic_clusterhdr{
	enum clusterType type;
	unsigned int cluster;
}__attribute__((packed));
struct epic_file{
	struct epic_clusterhdr clusterhdr;
	enum fileType type;
	char filename[128];
	unsigned int filesize;
	unsigned int parent;
	unsigned int last_file;
	unsigned int next_file;
	unsigned int first_file;
	unsigned int file_data;
}__attribute__((packed));
struct file{
	enum fsType fstype;
	enum fileType filetype;
	unsigned int file_cluster;
	unsigned char reserved[16];
};
int read_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int wordsPersector);
int write_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int wordsPersector);
int drive_getinfo(unsigned int drive, uint16_t* info);
int epic_get_fsinfo(unsigned int drive, struct epic_fshdr* pinfo);
struct file* openfile(unsigned int drive, const char* filename);
struct highlow_64 drive_getsectors(unsigned int drive);
unsigned int getbootdrive(void);
#endif
