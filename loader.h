#ifndef _LOADER
#define _LOADER
typedef int(*programEntry)(char** argp, unsigned int argc);
int loader_genargs(char* arg, char*** pppargs, unsigned int* pargc);
int load_elf(unsigned int drive, char* filename, char* arg);
int load_bin(unsigned int drive, char* filename, char* arg);
#endif
