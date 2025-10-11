#ifndef _ASM
#define _ASM
enum regType{
	REG_EAX = 0x0,
	REG_ECX = 0x1,
	REG_EDX = 0x2,
	REG_EBX = 0x3,
	REG_ESP = 0x4,
	REG_EBP = 0x5,
	REG_ESI = 0x6,
	REG_EDI = 0x7,
};
enum opType{
	OP_RET = 0xC3,
	OP_INT = 0xCD,
	OP_CLI = 0xFA,
	OP_STI = 0xFB,
	OP_HLT = 0xF4,
};
#endif
