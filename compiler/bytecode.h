#ifndef _BYTECODE
#define _BYTECODE
struct bytecode{
	unsigned char* pbuffer;
	unsigned int buffer_size;
}__attribute__((packed));
int bytecode_emit(struct bytecode* plist, unsigned int byte_cnt, ...);
struct bytecode* bytecode_init(unsigned int bytecode_size);
int bytecode_deinit(struct bytecode* pbytecode);
#endif
