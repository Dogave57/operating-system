#include "stdlib.h"
#include "filesystem.h"
#include "libsys.h"
typedef void(*shellFunc)(char* cmd, unsigned int cmdlen);
struct shelltab_entry{
	const char* name;
	unsigned int nameLen;
	shellFunc pfunc;
};
void shell_echo(char* cmd, unsigned int cmdlen);
void shell_run(char* cmd, unsigned int cmdlen);
void shell_clear(char* cmd, unsigned int cmdlen);
void shell_echo(char* cmd, unsigned int cmdlen){
	if (cmdlen<6)
		return;
	printf("%s\n", cmd+5);	
	return;
}
void shell_run(char* cmd, unsigned int cmdlen){
	if (cmdlen<4)
		return;
	unsigned int bootdrive = sys_getbootdrive();
	sys_loadelf(bootdrive, cmd+4);
	return;
}
void shell_clear(char* cmd, unsigned int cmdlen){
	sys_clear();
	return;
}
struct shelltab_entry shell_table[]={
	{"echo ", 5, shell_echo},
	{"run ", 4, shell_run},
	{"clear", 5, shell_clear},
};
int execute_cmd(char* cmd){
	if (!cmd)
		return -1;
	unsigned int cmdlen = strlen(cmd);
	unsigned int entrycnt = sizeof(shell_table)/sizeof(struct shelltab_entry);
	for (unsigned int i = 0;i<entrycnt;i++){
		struct shelltab_entry entry = shell_table[i];
		if (!entry.name||cmdlen<entry.nameLen)
			continue;
		if (memcmp((void*)cmd, (void*)entry.name, entry.nameLen)!=0)
			continue;
		entry.pfunc(cmd, cmdlen);
		return 0;
	}
	return -1;
}
int _start(void){
	sys_clear();
	unsigned int bootdrive = sys_getbootdrive();
	sys_print("dynamic interactive shell loaded!\n");	
	unsigned char* testalloc = sys_kmalloc(64);
	printf("allocated dynamic memory at %p\n", testalloc);
	sys_kfree((void*)testalloc);
	struct file* testfile = sys_openfile(bootdrive, "assets/fonts/font.txt");
	if (!testfile){
		printf("failed to open test file\n");
		return -1;
	}
	unsigned int filesize = sys_getfilesize(testfile);
	unsigned char* filebuf = (unsigned char*)sys_kmalloc(filesize);
	if (!filebuf){
		printf("failed to allocate memory for test buffer\n");
		return -1;
	}
	if (sys_readfile(testfile, filebuf)!=0){
		printf("failed to read test file\n");
		sys_closefile(testfile);
		sys_kfree((void*)filebuf);	
		return -1;
	}
	printf("file buf: %s\n", filebuf);
	sys_closefile(testfile);
	sys_kfree((void*)filebuf);
	while (1){
		char input[256] = {0};
		scan(input, sizeof(input)-1, '\n');
		execute_cmd(input);
	}
	return 0;
}
