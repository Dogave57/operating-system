#ifndef _COMMANDS
#define _COMMANDS
struct cmdmapping_t{
	const char* cmd;
	size_t cmdlen;
	void* handler;
};
int executecmd(char* cmd, size_t cmdleon);
void clearcmd(char* cmd, size_t cmdlen);
void echocmd(char* cmd, size_t cmdlen);
void rebootcmd(char* cmd, size_t cmdlen);
typedef void(*cmdfunc)(char* cmd, size_t cmdlen);
static struct cmdmapping_t cmdmap[]={
	{"clear\n", 5, clearcmd},
	{"echo", 4, echocmd},
	{"reboot\n", 6, rebootcmd},
};
#endif
