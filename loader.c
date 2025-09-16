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
	struct elf32_phdr* phdr_start = (struct elf32_phdr*)(filebuf+ehdr->ph_off);
	struct elf32_shdr* shdr_start = (struct elf32_shdr*)(filebuf+ehdr->sh_off);
	unsigned int imagesize = 0;
	printf("valid elf binary\n");
	printf("program headers: %d\n", ehdr->ph_cnt);
	for (unsigned int i = 0;i<ehdr->ph_cnt;i++){
		struct elf32_phdr* phdr = (struct elf32_phdr*)(phdr_start+i);
		if (phdr->p_type!=PT_LOAD)
			continue;
		printf("program header type: %d\n", phdr->p_type);
		imagesize+=phdr->p_memsz;
	}
	printf("image size: %d\n", imagesize);
	unsigned char* pimage = (unsigned char*)kmalloc(imagesize);
	if (!pimage){
		printf("failed to allocate memory for image\n");
		kfree((void*)filebuf);
		return -1;
	}	
	for (unsigned int i = 0;i<ehdr->ph_cnt;i++){
		struct elf32_phdr* phdr = (struct elf32_phdr*)(phdr_start+i);
		if (phdr->p_type!=PT_LOAD)
			continue;
		printf("copying segment with file offset %d and va %d\n", phdr->p_offset, phdr->p_va);
		memcpy((void*)(pimage+phdr->p_va), (const void*)(filebuf+phdr->p_offset), phdr->p_filesz);
	}
	printf("sh offset: %d\n", ehdr->sh_off);
	for (unsigned int i = 0;i<ehdr->sh_cnt;i++){
		struct elf32_shdr* shdr = (struct elf32_shdr*)(shdr_start+i);
		if (shdr->sh_type!=SHT_REL&&shdr->sh_type!=SHT_RELA)
			continue;
		printf("reloc entry found");
		unsigned int entrysize = 0;
		unsigned int entrycnt = 0;
		if (shdr->sh_type==SHT_REL)
			entrysize = sizeof(struct elf32_rel);
		else
			entrysize = sizeof(struct elf32_rela);
		entrycnt = shdr->sh_size/entrysize;
		for (unsigned int s = 0;s<entrycnt;s++){
		struct elf32_rel* pentry = (struct elf32_rel*)((pfile+shdr->sh_offset)+(s*entrysize));
		printf("reloc entry offset: %d", pentry->r_offset);	
		}
	}
	unsigned char* pentry = pimage+ehdr->entry;
	programEntry entry = (programEntry)(pentry);
	kfree((void*)filebuf);
	entry();
	printf("execution finished\n");
	return 0;
}
