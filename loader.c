#include "filesystem.h"
#include "stdlib.h"
#include "kernel.h"
#include "video.h"
#include "loader.h"
int load_elf(unsigned int drive, char* filename){
	if (!filename)
		return -1;
	struct file* pfile = openfile(drive, filename);
	if (!pfile)
		return -1;
	unsigned int filesize = getfilesize(pfile);
	if (!filesize){
		closefile(pfile);
		return -1;
	}
	unsigned char* filebuf = (unsigned char*)kmalloc(filesize);
	if (!filebuf){
		printf("failed to allocate memory for file buffer\n");
		closefile(pfile);
		return -1;
	}
	if (readfile(pfile, filebuf)!=0){
		printf("failed to read file into memory\n");
		closefile(pfile);
		return -1;	
	}
	closefile(pfile);
	if (!ISELF(filebuf)){
		printf("invalid elf binary\n");
		return -1;
	}
	kfree((void*)filebuf);
	return 0;
}
