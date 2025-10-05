#include "libsys.h"
#include "stdlib.h"
#include "musicedit.h"
typedef int(*cmdFunc)(char** pargs, unsigned int nargs);
struct cmd{
	const char* cmd;
	unsigned int cmdlen;
	cmdFunc func;
};
struct beep* beeplist = (struct beep*)0x0;
struct beep* lastbeep = (struct beep*)0x0;
int cmd_exit(char** pargs, unsigned int nargs);
int cmd_addbeep(char** pargs, unsigned int nargs);
int cmd_play(char** pargs, unsigned int nargs);
int cmd_clear(char** pargs, unsigned int nargs);
int run_cmd(char* cmd);
struct cmd cmdlist[] = {
	{"exit", 4, cmd_exit},
	{"addbeep", 7, cmd_addbeep},
	{"play", 4, cmd_play},
	{"clear", 5, cmd_clear},
};
static unsigned int exit = 0;
int cmd_exit(char** pargs, unsigned int nargs){	
	struct beep* currentbeep = beeplist;
	while (currentbeep){
		struct beep* flink = currentbeep->flink;
		sys_kfree((void*)currentbeep);
		currentbeep = flink;
	}
	exit = 1;
	return 0;	
}
int cmd_addbeep(char** pargs, unsigned int nargs){
	if (!pargs)
		return -1;
	if (nargs<2)
		return-1;
	unsigned int frequency = atoi(pargs[0]);
	unsigned int duration = atoi(pargs[1]);
	sys_beep(frequency, duration);
	struct beep* newbeep = (struct beep*)sys_kmalloc(sizeof(struct beep));
	if (!newbeep)
		return -1;
	newbeep->freq = frequency;
	newbeep->dur = duration;
	if (!beeplist){
		beeplist = newbeep;
		lastbeep = newbeep;
		return 0;
	}
	if (!lastbeep){
		lastbeep = newbeep;
		return 0;
	}
	lastbeep->flink = newbeep;
	newbeep->blink = lastbeep;
	lastbeep = newbeep;
	return 0;
}
int cmd_play(char** pargs, unsigned int nargs){
	struct beep* currentbeep = beeplist;
	while (currentbeep){
		sys_beep(currentbeep->freq, currentbeep->dur);
		currentbeep = currentbeep->flink;
	}
	return 0;
}
int cmd_clear(char** pargs, unsigned int nargs){
	sys_clear();
	return 0;
}
int run_cmd(char* cmd){
	if (!cmd)
		return -1;
	unsigned int cmdlen = strlen(cmd);
	unsigned int cmdEntries = sizeof(cmdlist)/sizeof(struct cmd);
	for (unsigned int i = 0;i<cmdEntries;i++){
		struct cmd entry = cmdlist[i];
		if (cmdlen<entry.cmdlen)
			continue;
		if (memcmp((void*)entry.cmd, (void*)cmd, entry.cmdlen)!=0)
			continue;
		unsigned int argc = 0;
		for (unsigned int i = entry.cmdlen;i<cmdlen;i++){
			if (cmd[i]==' ')
				argc++;
		}
		printf("arg count: %d\n", argc);	
		char** pargs = (char**)sys_kmalloc(sizeof(char*)*argc);
		unsigned int argindex = 0;
		unsigned int argstart = 0;
		if (pargs){
			for (unsigned int i = entry.cmdlen;i<cmdlen;i++){
				if (cmd[i]!=' ')
					continue;
				cmd[i] = 0;
				pargs[argindex] = cmd+i+1;
				argstart = i+1;
				argindex++;
			}	
		}
		entry.func(pargs, argc);
		if (pargs)
			sys_kfree((void*)pargs);
		return 0;
	}
	printf("Invalid command\n");
	return -1;	
}
int _start(void){
	sys_print("Welcome to the music editor!\n");
	char input[256] = {0};
	while (1){
		scan(input, sizeof(input)-1, '\n');
		run_cmd(input);
		if (exit){
			return 0;
		}
	}
	return 0;
}
