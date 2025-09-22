#include "filesystem.h"
#include "stdlib.h"
#include "kernel.h"
#include "video.h"
#include "elf.h"
#include "reb.h"
#include "timer.h"
#include "loader.h"
int load_elf(unsigned int drive, char* filename){
	if (!filename)
		return -1;
	unsigned int before_ms = time_ms;
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
		printf("not a valid ELF binary\n");
		return -1;
	}
	struct elf32_hdr* ehdr = (struct elf32_hdr*)filebuf;
	if (ehdr->type!=ET_DYN){
		printf("non-dynamic elf binary!\n");
		return -1;
	}
	struct elf32_shdr* shdr_start = (struct elf32_shdr*)(filebuf+ehdr->sh_off);
	struct elf32_phdr* phdr_start = (struct elf32_phdr*)(filebuf+ehdr->ph_off);
	unsigned int imagesize = 0;
	unsigned int imagedt = 0;
	for (unsigned int i = 0;i<ehdr->sh_cnt;i++){
		struct elf32_phdr* pentry = phdr_start+i;
		if (pentry->p_type!=PT_LOAD)
			continue;
		imagesize+=pentry->p_memsz;
	}
	unsigned char* pimage = (unsigned char*)kmalloc(imagesize);
	if (!pimage){
		kfree((void*)filebuf);
		return -1;
	}
	unsigned int prefered_base = 0;
	for (unsigned int i = 0;i<ehdr->ph_cnt;i++){
		struct elf32_phdr* pentry = phdr_start+i;
		if (pentry->p_type!=PT_LOAD)
			continue;
		memcpy((void*)(pimage+pentry->p_va), (const void*)(filebuf+pentry->p_offset), pentry->p_filesz);
	}
	imagedt = (unsigned int)pimage-prefered_base;
	for (unsigned int i = 0;i<ehdr->sh_cnt;i++){
		struct elf32_shdr* pentry = shdr_start+i;
		if (pentry->sh_type!=SHT_REL&&pentry->sh_type!=SHT_RELA)
			continue;
		unsigned int entrysize = 0;
		if (pentry->sh_type==SHT_REL)
			entrysize = sizeof(struct elf32_rel);
		else
			entrysize = sizeof(struct elf32_rela);
		unsigned int entrycnt = pentry->sh_size/entrysize;
		for (unsigned int s = 0;s<entrycnt;s++){
		struct elf32_rel* preldata = (struct elf32_rel*)(filebuf+pentry->sh_offset+(s*entrysize));
		unsigned int* ppatch = (unsigned int*)(pimage+preldata->r_offset);
		*ppatch+=imagedt;
		}
	}
	kfree((void*)filebuf);
	static programEntry entry = 0;
	entry = (programEntry)(pimage+ehdr->entry);
	printf("loaded program in %dms\n", time_ms-before_ms);
	entry();
	printf("program finished execution\n");
	kfree((void*)pimage);
	return 0;
}
int load_bin(unsigned int drive, char* filename){
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
	unsigned char* pimage = (unsigned char*)kmalloc(filesize);
	if (!pimage){
		closefile(pfile);
		return -1;
	}
	if (readfile(pfile, pimage)!=0){
		closefile(pfile);
		return -1;
	}
	closefile(pfile);
	programEntry entry = (programEntry)pimage;
	entry();
	kfree((void*)pimage);
	return 0;
}
