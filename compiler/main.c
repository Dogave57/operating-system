#include "../stdlib.h"
#include "../libsys.h"
#include "compiler.h"
#include "token.h"
#include "bytecode.h"
static unsigned int bootdrive = 0;
int init_tokenlist(struct tokenlist_t** pptokenlist){
	if (!pptokenlist)
		return -1;
	struct tokenlist_t* ptokenlist = (struct tokenlist_t*)sys_kmalloc(sizeof(struct tokenlist_t));
	if (!ptokenlist)
		return -1;
	memset((void*)ptokenlist, 0, sizeof(struct tokenlist_t));
	ptokenlist->pstart = (struct token_t*)0x0;
	ptokenlist->pend = (struct token_t*)0x0;
	ptokenlist->tokencnt = 0;
	*pptokenlist = ptokenlist;
	return 0;
}
int deinit_tokenlist(struct tokenlist_t* ptokenlist){
	if (!ptokenlist)
		return -1;
	struct token_t* ptoken = ptokenlist->pstart;
	while (ptoken){
		struct token_t* pflink = ptoken->flink;
		sys_kfree((void*)ptoken);
		ptoken = pflink;
	}
	sys_kfree((void*)ptokenlist);
	return 0;
}
struct token_t* add_token(struct tokenlist_t* ptokenlist, unsigned int tokenType){
	struct token_t* ptoken = (struct token_t*)0x0;
	if (!ptokenlist)
		return ptoken;
	ptoken = (struct token_t*)sys_kmalloc(sizeof(struct token_t));
	if (!ptoken)
		return ptoken;
	memset((void*)ptoken, 0, sizeof(struct token_t));
	if (!ptokenlist->pstart)
		ptokenlist->pstart = ptoken;
	ptoken->type = tokenType;
	ptokenlist->pend->flink = ptoken;
	ptoken->blink = ptokenlist->pend;
	ptokenlist->pend = ptoken;
	ptokenlist->tokencnt++;
	return ptoken;
}
int remove_token(struct tokenlist_t* ptokenlist, struct token_t* ptoken){
	if (!ptokenlist||!ptoken)
		return -1;
	struct token_t* pflink = ptoken->flink;
	struct token_t* pblink = ptoken->blink;
	pflink->blink = pblink;
	pblink->flink = pflink;
	ptokenlist->tokencnt--;
	sys_kfree((void*)ptoken);
	return 0;
}
int tokenize_file(char* filebuffer, struct tokenlist_t** pptokenlist){
	if (!filebuffer||!pptokenlist)
		return -1;
	unsigned int before_ms = sys_get_time_ms();
	struct tokenlist_t* ptokenlist = (struct tokenlist_t*)0x0;
	if (init_tokenlist(&ptokenlist)!=0||!ptokenlist){
		printf("failed to initialize token list\n");
		return -1;
	}
	unsigned int tokencnt = sizeof(token_map)/sizeof(struct token_mapping_t);
	for (unsigned int i = 0;filebuffer[i];i++){
		for (unsigned int tok_id = 0;tok_id<tokencnt;tok_id++){
			struct token_mapping_t mapping = token_map[tok_id];
			if (!mapping.name||!mapping.len||!mapping.type)
				continue;
			if (memcmp((void*)(filebuffer+i), (void*)mapping.name, mapping.len)!=0){
			continue;
			}
			struct token_t* pnewtoken = add_token(ptokenlist, mapping.type);
			if (!pnewtoken){
			printf("failed to create new token\n");
			continue;
			}
			if (mapping.type==TOKEN_EOI){
			ptokenlist->eoi_cnt++;
			}
			i+=mapping.len-1;
			break;
		}
	}
	struct token_t* current_token = ptokenlist->pstart;
	while (current_token){
		printf("token: %d ", current_token->type);
		current_token = current_token->flink;
	}
	unsigned int after_ms = sys_get_time_ms();
	unsigned int time_elapsed = after_ms-before_ms;
	printf("took %dms to tokenize\n", time_elapsed);
	*pptokenlist = ptokenlist;
	return 0;
}
int compile_file(char* filename){
	if (!filename)
		return -1;
	struct file* pfile = sys_openfile(bootdrive, filename);
	if (!pfile){
		printf("failed to open %s\n", filename);
		return -1;
	}
	unsigned int filesize = sys_getfilesize(pfile);
	char* filebuffer = (char*)sys_kmalloc(filesize+1);
	if (!filebuffer){
		printf("failed to allocate memory for code file %s\n", filename);
		sys_closefile(pfile);
		return -1;
	}
	if (sys_readfile(pfile, filebuffer)!=0){
		printf("failed to read file %s\n", filename);
		sys_closefile(pfile);
		sys_kfree((void*)filebuffer); 
		return -1;
	}
	filebuffer[filesize] = 0;
	sys_closefile(pfile);
	printf("successfully opened and read file %s\n", filename);
	printf("%s\n", filebuffer);
	struct tokenlist_t* ptokenlist = (struct tokenlist_t*)0x0;
	if (tokenize_file(filebuffer, &ptokenlist)!=0){
		printf("failed to tokenize code\n");
		sys_kfree((void*)filebuffer);
		return -1;
	}
	sys_kfree((void*)filebuffer);
	unsigned int bytecode_size = ptokenlist->eoi_cnt*8;
	printf("size: %d\n", bytecode_size);
	struct bytecode* pbytecode  = (struct bytecode*)bytecode_init(bytecode_size);
	if (!pbytecode){
		printf("failed to intialize bytecode list\n");
		deinit_tokenlist(ptokenlist);
		return -1;
	}
	printf("pbytecode: %p\n", (void*)pbytecode->pbuffer);
	printf("bytecode size: %d\n", pbytecode->buffer_size);
	if (deinit_tokenlist(ptokenlist)!=0){
		printf("failed to deinit token list\n");
		return -1;
	}
	if (bytecode_deinit(pbytecode)!=0){
		printf("failed to deinit bytecode\n");
		return -1;
	}
	return 0;
}
int _start(char** argp, unsigned int argc){
	if (!argp||argc<2){
		printf("no arguments provided\n");
		return -1;
	}
	bootdrive = sys_getbootdrive();
	unsigned int filecnt = argc-1;
	for (unsigned int i = 0;i<filecnt;i++){
		char* filename = argp[i+1];
		if (!filename)
			continue;
		if (compile_file(filename)!=0){
			printf("failed to compile file %s\n", filename);
			continue;
		}
	}
	return 0;
}
