#ifndef _FILESYSTEM
#define _FILESYSTEM
#define FS_SIGNATURE (unsigned int)0x43495045 //EPIC
#define FS_EOC (unsigned int)0xFFFF;
#define FS_FC (unsigned int)0xFFFE;
struct fs_hdr{
	unsigned int signature;
	unsigned int bytes_per_cluster;
	unsigned int fat_size;
	unsigned int data_off;
}__attribute__((packed));
#endif
