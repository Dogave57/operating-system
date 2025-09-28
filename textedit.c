#include "libsys.h"
#include "stdlib.h"
int _start(char** argp, unsigned int argc){
	if (!argp)
		return -1;
	if (argc<2){
		return -1;
	}
	char* filename = argp[1];
	printf("loading file %s\n", filename);
	printf("Welcome to text edit\n");
	return 0;
}
