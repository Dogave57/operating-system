#ifndef _FILESYSTEM
#define _FILESYSTEM
#define FS_SIGNATURE 0x43495045 //EPIC
struct fs_hdr{
	unsigned int signature;
	unsigned int bytes_per_cluster;
	unsigned int fat_size;
	unsigned int data_off;
}__attribute__((packed));
#endif
