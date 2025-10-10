#include "../stdlib.h"
#include "../libsys.h"
#include "bytecode.h"
#include "compiler.h"
__attribute__((cdecl)) struct bytecode* bytecode_init(unsigned int bytecodesize){
	if (!bytecodesize)
		return (struct bytecode*)0x0;
	struct bytecode* pbytecode = (struct bytecode*)sys_kmalloc(sizeof(struct bytecode));
	if (!pbytecode){
		return (struct bytecode*)0x0;
	}
	pbytecode->pbuffer = (unsigned char*)sys_kmalloc(bytecodesize);
	if (!pbytecode->pbuffer){
		printf("bytecode size: %d\n", bytecodesize);
		sys_kfree((void*)pbytecode);
		return (struct bytecode*)0x0;
	}
	pbytecode->buffer_used = 0;
	pbytecode->buffer_size = bytecodesize;
	return pbytecode;
}
__attribute__((cdecl)) int bytecode_deinit(struct bytecode* pbytecode){
	if (!pbytecode)
		return -1;
	sys_kfree((void*)pbytecode->pbuffer);
	sys_kfree((void*)pbytecode);
	return 0;
}
__attribute__((cdecl)) int bytecode_emit(struct bytecode* pbytecode, unsigned int bytecode_size, ...){
	if (!pbytecode||!pbytecode)
		return -1;
	unsigned char* pcode = (unsigned char*)((&bytecode_size)+1);
	for (unsigned int i = pbytecode->buffer_used;i<pbytecode->buffer_used+bytecode_size;i++){
		unsigned char code = *pcode;
		pbytecode->pbuffer[i] = (unsigned char)code;
		pcode+=4;
	}
	pbytecode->buffer_used+=bytecode_size;
	return 0;
}
