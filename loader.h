#ifndef _LOADER
#define _LOADER
#define ISELF(buf)((*(buf)=='E')&&(*(buf+1)=='L')&&(*(buf+2)=='F'))
int load_elf(unsigned int drive, char* filename);
#endif
