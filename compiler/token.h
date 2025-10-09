#ifndef _TOKEN
#define _TOKEN
enum tokenType{
	TOKEN_INVALID,
	TOKEN_MOV,
	TOKEN_EAX,
	TOKEN_EBX,
	TOKEN_ECX,
	TOKEN_EDX,
	TOKEN_EDI,
	TOKEN_ESI,
	TOKEN_ESP,
	TOKEN_EBP,
	TOKEN_RET,
	TOKEN_EOI,
	TOKEN_SUB,
	TOKEN_ADD,
};
struct token_mapping_t{
	char* name;
	unsigned int len;
	enum tokenType type;
};
struct token_t{
	enum tokenType type;
	struct token_t* flink;
	struct token_t* blink;
	unsigned char metadata[32];
};
struct tokenlist_t{
	struct token_t* pstart;
	struct token_t* pend;
	unsigned int tokencnt;
	unsigned int eoi_cnt;
}; 
struct token_mapping_t token_map[] = {
	{"mov", 3, TOKEN_MOV},
	{"eax", 3, TOKEN_EAX},
	{"ebx", 3, TOKEN_EBX},
	{"ecx", 3, TOKEN_ECX},
	{"edx", 3, TOKEN_EDX},
	{"edi", 3, TOKEN_EDI},
	{"esi", 3, TOKEN_ESI},
	{"esp", 3, TOKEN_ESP},
	{"ebp", 3, TOKEN_EBP},
	{"ret", 3, TOKEN_RET},
	{";", 1, TOKEN_EOI},
	{"\n", 1, TOKEN_EOI},
	{"sub", 3, TOKEN_SUB},
	{"add", 3, TOKEN_ADD},
};
#endif
