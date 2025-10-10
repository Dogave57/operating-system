#ifndef _BYTECODE
#define _BYTECODE
struct bytecode{
	unsigned char* pbuffer;
	unsigned int buffer_size;
	unsigned int buffer_used;
}__attribute__((packed));
__attribute__((cdecl))int bytecode_emit(struct bytecode* plist, unsigned int byte_cnt, ...);
struct bytecode* bytecode_init(unsigned int bytecode_size);
int bytecode_deinit(struct bytecode* pbytecode);
#endif
