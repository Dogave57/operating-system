#ifndef _LOADER
#define _LOADER
typedef int(*programEntry)(void);
int load_elf(unsigned int drive, char* filename);
int load_bin(unsigned int drive, char* filename);
#endif
