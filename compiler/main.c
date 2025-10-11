#include "../stdlib.h"
#include "../libsys.h"
#include "../reb.h"
#include "compiler.h"
#include "token.h"
#include "asm.h"
#include "bytecode.h"
int build_reb(struct bytecode* pbytecode, char* filename);
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
	unsigned int tokencnt = (sizeof(token_map)/sizeof(struct token_mapping_t));
	for (unsigned int i = 0;filebuffer[i];i++){
		unsigned char ch = filebuffer[i];
		if (ch==' ')
			continue;
		enum tokenType tokenType = TOKEN_INVALID;
		for (unsigned int tok_id = 0;tok_id<tokencnt;tok_id++){
			struct token_mapping_t mapping = token_map[tok_id];
			if (!mapping.name)
				continue;
			if (memcmp((void*)(filebuffer+i), (void*)mapping.name, mapping.len)!=0)
				continue;
			struct token_t* pnewtoken = add_token(ptokenlist, mapping.type);
			if (!pnewtoken){
				printf("failed to add token\n");
				continue;
			}
			if (mapping.type==TOKEN_EOI){
				ptokenlist->eoi_cnt++;
			}
			tokenType = mapping.type;
			i+=mapping.len-1;
			printf("token %d", mapping.type);
			break;
		}
		if (tokenType!=TOKEN_INVALID)
			continue;
		unsigned int dword = 0;
		unsigned int fail = 0;
		unsigned int hex = (filebuffer[i]=='0'&&filebuffer[i+1]=='x');
		unsigned int hex_position = 0;
		if (hex)
			i+=2;
		for (;filebuffer[i];i++){
			if (hex&&hex_position>8)
				break;
			unsigned char ch = filebuffer[i];
			if (ch==','||ch=='\n'||ch==';')
				break;
			if ((ch>'A'-1&&ch<'F'+1)&&hex){
				unsigned char digit = ch-'A'+10;
				dword = dword * 16 + digit;
				continue;	
			}
			if ((ch>'a'-1&&ch<'f'+1)&&hex){
				unsigned char digit = ch-'a'+10;
				dword = dword * 16 + digit;
				continue;
			}
			if (hex){
				unsigned char digit = ch-'0';
				dword = dword * 16 + digit;
				continue;
			}
			if (ch<'0'||ch>'9'){
				fail = 1;
				printf("unexpected '%c'\n", ch);
				break;
			}
			unsigned char digit = ch-'0';
			dword = dword * 10 + digit;
		}
		if (fail){
			printf("tokenization failed\n");
			return -1;
		}
		printf("dword: %x\n", dword);
		struct token_t* pnewtoken = add_token(ptokenlist, TOKEN_DWORD);
		if (!pnewtoken){
			printf("failed to add DWORD token\n");
			continue;
		}
		*((unsigned int*)pnewtoken->metadata) = dword;
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
	struct token_t* ptoken = ptokenlist->pstart;
	unsigned int fail = 0;
	while (ptoken){
		switch (ptoken->type){
			case TOKEN_RET:{
			printf("emitting ret");
			bytecode_emit(pbytecode, 1, OP_RET);
			break;
			}
			case TOKEN_INT:{
			struct token_t* argtoken = ptoken->flink;
		        if (!argtoken||argtoken->type!=TOKEN_DWORD){
				printf("expected byte after INT instruction\n");
				fail = 1;
				break;
			}	
			bytecode_emit(pbytecode, 2, OP_INT, argtoken->metadata[0]);			
			break;		
       		       	}
			case TOKEN_STI:{
			bytecode_emit(pbytecode, 1, OP_STI);
			break;
			};
			case TOKEN_CLI:{
			bytecode_emit(pbytecode, 1, OP_CLI);
			break;
			};
			case TOKEN_HLT:{
			bytecode_emit(pbytecode, 1, OP_HLT);
			break;
			};
		}
		if (fail)
			break;
		ptoken = ptoken->flink;
	}
	if (fail){
		printf("compilation failed\n");
		deinit_tokenlist(ptokenlist);
		bytecode_deinit(pbytecode);
		return -1;
	}
	if (build_reb(pbytecode, "output.reb")!=0){
		printf("failed to build relocatable executable binary\n");
		deinit_tokenlist(ptokenlist);
		bytecode_deinit(pbytecode);
		return -1;
	}
	if (deinit_tokenlist(ptokenlist)!=0){
		printf("failed to deinit token list\n");
		bytecode_deinit(pbytecode);
		return -1;
	}
	if (bytecode_deinit(pbytecode)!=0){
		printf("failed to deinit bytecode\n");
		return -1;
	}
	return 0;
}
int build_reb(struct bytecode* pbytecode, char* filename){
	if (!pbytecode||!filename)
		return -1;
	if (!pbytecode->pbuffer||!pbytecode->buffer_used||!pbytecode->buffer_size){
		printf("invalid bytecode buffer\n");
		printf("pbuffer: %p\n", (void*)pbytecode->pbuffer);
		return -1;
	}
	unsigned char* pcode = pbytecode->pbuffer;
	unsigned int binary_size = pbytecode->buffer_used+sizeof(struct reb32_hdr);
	unsigned int bytecode_size = pbytecode->buffer_used;
	bytecode_size+=4-(bytecode_size%4);
	unsigned int imagesize = sizeof(struct reb32_hdr)+bytecode_size;
	imagesize+=4-(imagesize%4);
	struct reb32_hdr* pimage = (struct reb32_hdr*)sys_kmalloc(imagesize);
	unsigned char* imgdata = pimage->imgdata;
	pimage->signature = REB_SIGNATURE;
	pimage->type = REB_TYPE_EXEC;
	pimage->prefered_base = (unsigned int)pimage;
	pimage->bss_size = 0;
	pimage->entry_off = 0;
	pimage->reloc_entrycnt = 0;
	memcpy((void*)(imgdata), (void*)pbytecode->pbuffer, bytecode_size);
	struct file* pfile = (struct file*)sys_openfile(bootdrive, filename);
	if (!pfile){
		sys_createfile(bootdrive, filename, FILE_REGULAR);
		pfile = (struct file*)sys_openfile(bootdrive, filename);
		if (!pfile){
			printf("failed to open output file\n");
			sys_kfree((void*)pimage);
			return -1;
		}
	}
	if (sys_writefile(pfile, (unsigned char*)pimage, imagesize)!=0){
		printf("failed to write to output file\n");
		sys_kfree((void*)pimage);
		sys_closefile(pfile);
		return -1;
	}
	sys_closefile(pfile);
	printf("successfully created a relocatable executable binary\n");
	return 0;
}
int _start(char** argp, unsigned int argc){
	if (!argp||argc<1){
		printf("no arguments provided\n");
		return -1;
	}
	bootdrive = sys_getbootdrive();
	unsigned int filecnt = argc;
	for (unsigned int i = 0;i<filecnt;i++){
		char* filename = argp[i];
		if (!filename)
			continue;
		if (compile_file(filename)!=0){
			printf("failed to compile file %s\n", filename);
			continue;
		}
	}
	return 0;
}
