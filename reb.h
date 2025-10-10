#ifndef _REB
#define _REB
#define REB_SIGNATURE 0x52454200 
#define REB_TYPE_INVALID 0x0
#define REB_TYPE_EXEC 0x1
//REB+NULL
#define ISREB(buf)(*((unsigned int*)buf)==REB_SIGNATURE)
struct reb32_hdr{
	unsigned int signature;
	unsigned int type;
	unsigned int entry_off;
	unsigned int prefered_base;
	unsigned int bss_size;
	unsigned int reloc_entrycnt;
	unsigned char imgdata[];
}__attribute__((packed));
struct reb32_relentry{
	uint32_t roff;
}__attribute__((packed));
#endif
