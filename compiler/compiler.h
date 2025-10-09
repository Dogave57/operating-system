#ifndef _COMPILER
#define _COMPILER
#include "token.h"
int init_tokenlist(struct tokenlist_t** pptokenlist);
int deinit_tokenlist(struct tokenlist_t* ptokenlist);
struct token_t* add_token(struct tokenlist_t* ptokenlist, unsigned int tokenType);
int remove_token(struct tokenlist_t* ptokenlist, struct token_t* ptoken);
int compile_file(char* filename);
#endif
