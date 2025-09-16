#ifndef _ELF
#define _ELF
#include <stdint.h>
#define ISELF(buf)((*(buf)==0x7F)&&(*(buf+1)=='E')&&(*(buf+2)=='L')&&(*(buf+3)=='F'))
#define EM_I386 3
#define PT_LOAD 1
#define SHT_REL 9
#define SHT_RELA 4
enum elfType{
	ET_NONE = 0,	
	ET_REL = 1,
	ET_EXEC = 2,
};
struct elf32_hdr{
	unsigned char ident[16];
	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint32_t entry;
	uint32_t ph_off;
	uint32_t sh_off;
	uint32_t flags;
	uint16_t hdr_size;
	uint16_t ph_size;
	uint16_t ph_cnt;
	uint16_t sh_size;
	uint16_t sh_cnt;
	uint16_t strtab_index;
};
struct elf32_shdr{
	uint32_t sh_name;
	uint32_t sh_type;
	uint32_t sh_flags;
	uint32_t sh_addr;
	uint32_t sh_offset;
	uint32_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint32_t sh_addralign;
	uint32_t sh_entrysize;
};
struct elf32_phdr{
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_va;
	uint32_t p_pa;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
};
struct elf32_sym{
	uint32_t st_name;
	uint32_t st_value;
	uint32_t st_size;
	uint8_t st_info;
	uint8_t st_other;
	uint16_t st_shndex;
};
struct elf32_rel{
	uint32_t r_offset;
	uint32_t r_info;
};
struct elf32_rela{
	uint32_t r_offset;
	uint32_t r_info;
	int32_t r_addend;
};
#endif
