#ifndef _FILESYSTEM
#define _FILESYSTEM
struct fs_link{
	char filename[32];
	unsigned int linksize;
	unsigned int index;
	unsigned int parent;
	unsigned int left;
	unsigned int right;
	char data[1];
};
int read_sector(unsigned int drive, unsigned int sector, unsigned int sectorcnt, void* buffer);
unsigned int getbootdrive(void);
#endif
