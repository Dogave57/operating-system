#ifndef _LOADER
#define _LOADER
typedef __attribute__((cdecl))int(*programEntry)(char** argp, unsigned int argc);
struct procinfo_t{
	void* pentry;
	void* pimage;
	char** argp;
	unsigned int argc;
};
int proc_bootstrap(struct procinfo_t* parg);
int loader_genargs(char* arg, char*** pppargs, unsigned int* pargc);
int load_binary(unsigned int drive, char* filename, char* arg);
int load_elf(unsigned int drive, char* filename, char* arg);
int load_reb(unsigned int drive, char* filename, char* arg);
int load_bin(unsigned int drive, char* filename, char* arg);
#endif
