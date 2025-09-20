#ifndef _REB
#define _REB
#define REB_SIGNATURE 0x52454200 
//REB+NULL
#define ISREB(buf)(*((unsigned int*)buf)==REB_SIGNATURE)
struct reb32_hdr{
	unsigned int signature;
	unsigned int reloc_entries;
	unsigned int bss_size;
}__attribute__((packed));
struct reb32_relentry{
	uint32_t roff;
}__attribute__((packed));
#endif
