#ifndef _LOADER
#define _LOADER
typedef int(*programEntry)(void);
int load_elf(unsigned int drive, char* filename);
#endif
