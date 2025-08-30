#ifndef _FILESYSTEM
#define _FILESYSTEM
#define EPICFS_SIGNATURE (unsigned int)0x43495045 //EPIC
#define EPICFS_EOC (unsigned int)0x0;
#define EPICFS_FC (unsigned int)0xFFFE;
struct fs_hdr{
	unsigned int signature;
	unsigned int bytes_per_cluster;
	unsigned int fat_size;
	unsigned int data_off;
}__attribute__((packed));
#endif
