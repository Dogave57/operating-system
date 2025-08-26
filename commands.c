#include <stdint.h>
#include <stddef.h>
#include "video.h"
#include "stdlib.h"
#include "kernel.h"
#include "commands.h"
int executecmd(char* cmd, size_t cmdlen){
	if (!cmd||cmdlen<1)
		return -1;
	for (unsigned int i = 0;i<sizeof(cmdmap)/sizeof(cmdmap[0]);i++){
		struct cmdmapping_t cmdmapping = cmdmap[i];
		if (cmdlen<cmdmapping.cmdlen)
			continue;
		if (memcmp((void*)cmd, (void*)cmdmapping.cmd, cmdmapping.cmdlen)!=0)
			continue;
		cmdfunc func = (cmdfunc)cmdmapping.handler;
		func(cmd+cmdmapping.cmdlen+1, cmdlen-cmdmapping.cmdlen-1);
		return 0;
	}
	print("command not found\n");
	return -1;
}
void clearcmd(char* cmd, size_t cmdlen){
	clear();
	return;
}
void echocmd(char* cmd, size_t cmdlen){
	print(cmd);
	return;
}
void rebootcmd(char* cmd, size_t cmdlen){
	reboot();
	return;
}
