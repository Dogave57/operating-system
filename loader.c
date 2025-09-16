#include "filesystem.h"
#include "stdlib.h"
#include "kernel.h"
#include "video.h"
#include "elf.h"
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
	struct elf32_hdr* ehdr = (struct elf32_hdr*)(filebuf);
	if (ehdr->type!=ET_EXEC){
		printf("elf binary is not executable!\n");
		return -1;
	}
	if (ehdr->machine!=EM_I386){
		printf("elf binary is not x86\n");
		return -1;
	}
	struct elf32_phdr* phdr_start = (struct elf32_phdr*)(ehdr+1);
	printf("valid elf binary\n");
	printf("program headers: %d\n", ehdr->ph_cnt);
	for (unsigned int i = 0;i<ehdr->ph_cnt;i++){
		struct elf32_phdr* phdr = (struct elf32_phdr*)(phdr_start+i);
		printf("program header type: %d\n", phdr->p_type);
	}
	kfree((void*)filebuf);
	return 0;
}
