#include "stdlib.h"
#include "filesystem.h"
#include "libsys.h"
int _start(void){
	sys_clear();
	unsigned int bootdrive = sys_getbootdrive();
	sys_print("dynamic interactive shell loaded!\n");	
/*	unsigned char* testalloc = sys_kmalloc(64);
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
	sys_kfree((void*)filebuf);*/
	char input[256] = {0};
	while (1){
		scan(input, sizeof(input)-1, '\n');
		printf("input: %s\n", input);
	}
	return 0;
}
