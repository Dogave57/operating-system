#ifndef _TOKEN
#define _TOKEN
enum tokenType{
	TOKEN_INVALID = 0,
	TOKEN_MOV = 1,
	TOKEN_EAX = 2,
	TOKEN_EBX = 3,
	TOKEN_ECX = 4,
	TOKEN_EDX = 5,
	TOKEN_EDI = 6,
	TOKEN_ESI = 7,
	TOKEN_ESP = 8,
	TOKEN_EBP = 9,
	TOKEN_RET = 10,
	TOKEN_EOI = 11,
	TOKEN_SUB = 12,
	TOKEN_ADD = 13,
	TOKEN_DWORD = 14,
	TOKEN_SEPERATOR = 15,
	TOKEN_INT = 16,
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
	unsigned char metadata[16];
};
struct tokenlist_t{
	struct token_t* pstart;
	struct token_t* pend;
	unsigned int tokencnt;
	unsigned int eoi_cnt;
}; 
static struct token_mapping_t token_map[] = {
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
	{",", 1, TOKEN_SEPERATOR},
	{"int", 3, TOKEN_INT},
};
#endif
