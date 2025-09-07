#ifndef _FILESYSTEM
#define _FILESYSTEM
#include <stdint.h>
#include "kernel.h"
#define FS_RESERVED_SECTORS 128
#define EPICFS_SIGNATURE (unsigned int)0x43495045 //EPIC
#define EPICFS_EOC (unsigned int)0xFFFFFFFF
#define EPICFS_FC (unsigned int)0x0
enum fsType{
	FS_NONE,
	FS_EPIC,
};
enum clusterType{
	CLUSTER_FILEDATA,
	CLUSTER_FILE,
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
	unsigned int freelist_size;
	unsigned int freelist_off;
	unsigned int data_off;
	unsigned int last_filemd_cluster;
}__attribute__((packed));
struct epic_clusterhdr{
	enum clusterType type;
	unsigned int cluster;
}__attribute__((packed));
struct epic_file{
	enum fileType type;
	char filename[32];
	unsigned int size;
	unsigned int data;
	unsigned int last_data_cluster;
	unsigned int file_cluster;
	unsigned int file_offset;
	unsigned int inuse;
}__attribute__((aligned(64)));
struct file{
	enum fsType fstype;
	enum fileType filetype;
	unsigned int len;
	unsigned int drive;
	unsigned int file_cluster;
	unsigned int file_offset;
	unsigned char reserved[16];
};
int read_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int wordsPersector);
int write_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int wordsPersector);
int drive_getinfo(unsigned int drive, uint16_t* info);
int epic_get_fsinfo(unsigned int drive, struct epic_fshdr* pinfo);
int epic_alloc_cluster(unsigned int drive, unsigned int* pcluster);
int epic_free_cluster(unsigned int drive, unsigned int cluster);
int epic_writecluster(unsigned int drive, unsigned int cluster, unsigned int data);
int epic_readcluster(unsigned int drive, unsigned int cluster, unsigned int* pdata);
int epic_write_clusterdata(unsigned int drive, unsigned int cluster, unsigned char* pdata);
int epic_read_clusterdata(unsigned int drive, unsigned int cluser, unsigned char* pdata);
struct file* openfile(unsigned int drive, const char* filename);
struct file* opendir(unsigned int drive, const char* dirname);
int renamefile(struct file* pfile, const char* newname);
int createfile(unsigned int drive, const char* filename);
int deletefile(struct file* pfile);
int readfile(struct file* pfile, unsigned char* buffer);
int writefile(struct file* pfile, unsigned char* buffer, unsigned int size);
unsigned int getfilesize(struct file* pfile);
int closefile(struct file* pfile);
struct highlow_64 drive_getsectors(unsigned int drive);
unsigned int getbootdrive(void);
#endif
