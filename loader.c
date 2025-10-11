#include "filesystem.h"
#include "stdlib.h"
#include "kernel.h"
#include "video.h"
#include "elf.h"
#include "reb.h"
#include "timer.h"
#include "thread.h"
#include "loader.h"
int loader_genargs(char* arg, char*** pppargs, unsigned int* pargc){
	if (!arg||!pppargs||!pargc)
		return -1;
	unsigned int argc = 0;
	unsigned int arglen = 1;
	for (;;arglen++){
		if (arg[arglen]!=' '&&arg[arglen]!=0){
			continue;
		}
		argc++;
		if (!arg[arglen]){
			break;
		}
	}
	if (!argc)
		return -1;
	printf("%s len %d cnt %d\n", arg, arglen, argc);
	unsigned int pargsize = (sizeof(char*)*argc)+arglen;
	char** ppargs = (char**)kmalloc(pargsize);
	if (!ppargs)
		return -1;
	char* parg = ((char*)ppargs+(sizeof(char*)*argc));
	memset((void*)parg, 0, arglen);
	unsigned int argindex = 0;
	unsigned int argstart = 0;
	for (unsigned int i = 0;;i++){
		char ch = arg[i];
		if (ch!=' '&&ch){
			parg[i] = ch;
			continue;
		}
		parg[i] = 0;
		ppargs[argindex] = parg+argstart;
		argindex++;
		argstart = i+1;
		if (!ch)
			break;
	}
	*pppargs = ppargs;
	*pargc = argc;
	return 0;
}
int load_binary(unsigned int drive, char* filename, char* arg){
	if (!filename||!arg)
		return -1;
	unsigned int filenamelen = strlen(filename);
	char* extension = filename+filenamelen-4;
	int status = -1;
	if (filenamelen>3&&memcmp((void*)extension, ".elf", 5)==0){
		status = load_elf(drive, filename, arg);
	}
	if (filenamelen>3&&memcmp((void*)extension, ".reb", 5)==0)
		status = load_reb(drive, filename, arg);
	return status;
}
int load_elf(unsigned int drive, char* filename, char* arg){
	if (!filename||!arg)
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
	programEntry entry = 0;
	entry = (programEntry)(pimage+ehdr->entry);
	printf("program loaded at %p\n", (void*)pimage);
	unsigned int argc = 0;
	char** argp = (char**)0x0;
	loader_genargs(arg, &argp, &argc);
	struct procinfo_t* pinfo = (struct procinfo_t*)kmalloc(sizeof(struct procinfo_t));
	if (!pinfo){
		printf("failed to allocate memory for process info\n");
		kfree((void*)pimage);
		kfree((void*)argp);
		return -1;
	}
	pinfo->argp = argp;
	pinfo->argc = argc;
	pinfo->pimage = (void*)pimage;
	pinfo->pentry = (void*)(entry);
	entry(argp, argc);
	kfree((void*)pimage);
	kfree((void*)argp);
	return 0;
	struct thread_t* pinit_thread = (struct thread_t*)thread_create((uint32_t)(proc_bootstrap), 0x1000, (void*)pinfo);
	if (!pinit_thread){
		printf("failed to create bootstrap thread\n");
		kfree((void*)pimage);
		kfree((void*)argp);
		return -1;
	}
	printf("switching to bootstrapper task\n");
	switch_task((struct thread_t*)0x0);
	printf("program finished\n");
	return 0;
}
int load_reb(unsigned int drive, char* filename, char* arg){
	if (!filename||!arg)	
		return -1;
	struct file* pfile = openfile(drive, filename);
	if (!pfile)
		return -1;
	unsigned int filesize = getfilesize(pfile);
	unsigned char* pimage = (unsigned char*)kmalloc(filesize);
	if (!pimage){
		closefile(pfile);
		return -1;
	}	
	if (readfile(pfile, pimage)!=0){
		closefile(pfile);
		kfree((void*)pimage);
		return -1;
	}
	closefile(pfile);
	struct reb32_hdr* phdr = (struct reb32_hdr*)pimage;
	if (!ISREB((unsigned char*)phdr)){
		printf("invalid reb binary\n");
		kfree((void*)pimage);
		return -1;
	}
	programEntry entry = (programEntry)((phdr->imgdata+phdr->entry_off));
	unsigned int argc = 0;
	char** argp = (char**)0x0;
	loader_genargs(arg, &argp, &argc);
	entry(argp, argc);
	printf("program finished execution\n");
	kfree((void*)argp);
	kfree((void*)pimage);
	return 0;
}
int load_bin(unsigned int drive, char* filename, char* arg){
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
	unsigned int argc = 0;
	char** argp = (char**)0x0;
	entry(argp, argc);
	kfree((void*)pimage);
	return 0;
}
