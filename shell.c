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
void shell_file(char* cmd, unsigned int cmdlen);
void shell_rf(char* cmd, unsigned int cmdlen);
void shell_cf(char* cmd, unsigned int cmdlen);
void shell_cd(char* cmd, unsigned int cmdlen);
void shell_df(char* cmd, unsigned int cmdlen);
void shell_wf(char* cmd, unsigned int cmdlen);
void shell_lf(char* cmd, unsigned int cmdlen);
void shell_color(char* cmd, unsigned int cmdlen);
void shell_help(char* cmd, unsigned int cmdlen);
void shell_echo(char* cmd, unsigned int cmdlen){
	if (cmdlen<6||!cmd)
		return;
	printf("%s\n", cmd+5);	
	return;
}
void shell_run(char* cmd, unsigned int cmdlen){
	if (cmdlen<4||!cmd)
		return;
	unsigned int bootdrive = sys_getbootdrive();
	sys_loadelf(bootdrive, cmd+4);
	return;
}
void shell_clear(char* cmd, unsigned int cmdlen){
	sys_clear();
	return;
}
void shell_file(char* cmd, unsigned int cmdlen){
	if (cmdlen<6||!cmd)
		return;
	struct fileinfo info = {0};
	char* filename = cmd+5;
	unsigned int bootdrive = sys_getbootdrive();
	struct file* pfile = sys_openfile(bootdrive, filename);
	if (!pfile)
		return;
	if (sys_getfileinfo(pfile, &info)!=0){
		printf("failed to get file info\n");
		sys_closefile(pfile);
		return;
	}
	printf("file name: %s\n", info.filename);
	printf("file size: %d bytes\n", info.filesize);
	printf("file drive: %d\n", info.drive);
	sys_closefile(pfile);
	return;
}
void shell_rf(char* cmd, unsigned int cmdlen){
	if (!cmd||cmdlen<3)
		return;
	unsigned int bootdrive = sys_getbootdrive();
	char* filename = cmd+3;
	struct file* pfile = sys_openfile(bootdrive, filename);
	if (!pfile){
		printf("invalid file\n");
		return;
	}
	unsigned int filesize = sys_getfilesize(pfile);
	char* filebuf = (char*)sys_kmalloc(filesize);
	if (!filebuf){
		sys_closefile(pfile);
		return;
	}
	if (sys_readfile(pfile, filebuf)!=0){
		printf("failed to read file\n");
		sys_closefile(pfile);
		sys_kfree((void*)filebuf);
		return;
	}
	sys_closefile(pfile);
	printf("%s\n", filebuf);
	sys_kfree((void*)filebuf);
	return;
}
void shell_cf(char* cmd, unsigned int cmdlen){
	if (!cmd||cmdlen<3)
		return;
	unsigned int bootdrive = sys_getbootdrive();
	char* filename = cmd+3;
	if (sys_createfile(bootdrive, filename, FILE_REGULAR)!=0){
		printf("failed to create file\n");
		return;
	}
	printf("successfully created %s\n", filename);
	return;
}
void shell_cd(char* cmd, unsigned int cmdlen){
	if (!cmd||cmdlen<3)
		return;
	unsigned int bootdrive = sys_getbootdrive();
	char* dirname = cmd+3;
	if (sys_createfile(bootdrive, dirname, FILE_DIR)!=0){
		printf("failed to create dir\n");
		return;
	}
	printf("successfully created %s\n", dirname);
	return;
}
void shell_df(char* cmd, unsigned int cmdlen){
	if (!cmd||cmdlen<3)
		return;
	unsigned int bootdrive = sys_getbootdrive();
	char* filename = cmd+3;
	struct file* pfile = sys_openfile(bootdrive, filename);
	if (!pfile)
		return;
	if (sys_deletefile(pfile)!=0){
		printf("failed to remove file\n");
		sys_closefile(pfile);
		return;
	}
	printf("successfully removed %s\n", filename);
	sys_closefile(pfile);
	return;
}
void shell_wf(char* cmd, unsigned int cmdlen){
	if (!cmd||cmdlen<3)
		return;
	unsigned int bootdrive = sys_getbootdrive();
	char* filename = cmd+3;
	char* filedata = filename;
	unsigned int filedata_off = 0;
	for (filedata_off = 0;*(filedata+filedata_off)!=' '&&filedata_off<cmdlen;filedata_off++){};
	if (!filedata_off)
		return;
	filedata_off++;
	filedata+=filedata_off;
	*(filedata-1) = 0;
	struct file* pfile = sys_openfile(bootdrive, filename);
	if (!pfile){
		printf("invalid file\n");
		return;
	}
	if (sys_writefile(pfile, filedata, cmdlen-filedata_off)!=0){
		printf("failed to write to file\n");
		return;
	}
	printf("successfully written to %s\n", filename);
	sys_closefile(pfile);
	return;
}
void shell_lf(char* cmd, unsigned int cmdlen){
	if (!cmd||cmdlen<3)
		return;
	unsigned int bootdrive = sys_getbootdrive();
	char* dirname = cmd+3;
	unsigned int isroot = (*(dirname)=='/')&&(*(dirname+1)==0);
	struct file* pdir = sys_openfile(bootdrive, dirname);
	if (!pdir&&!isroot){
		printf("invalid dir\n");
		return;
	}	
	struct fileinfo* pfilelist = (struct fileinfo*)0x0;
	unsigned int pfile_entries = 0;
	if (sys_getfilelist(bootdrive, pdir, &pfilelist, &pfile_entries)!=0||!pfilelist){
		printf("failed to get file list\n");
		sys_closefile(pdir);
		return;
	}
	for (unsigned int i = 0;i<pfile_entries;i++){
		struct fileinfo* pentry = pfilelist+i;
		printf("%s | %d bytes\n", pentry->filename, pentry->filesize);
	}
	sys_kfree((void*)pfilelist);
	sys_closefile(pdir);
	return;
}
void shell_color(char* cmd, unsigned int cmdlen){
	if (!cmd||cmdlen<6)
		return;
	char* strcolor = cmd+6;
	unsigned int color = atoi(strcolor);
	sys_set_console_color(color, VGA_COLOR_BLACK);
	return;
}
void shell_help(char* cmd, unsigned int cmdlen){
	printf("echo - print something out to the console\nrun - run a program\nclear - clear console\nfile - get file info\nrf - read file contents\ncf - create file\ndf - remove file\nwf - write file\nlf - list files\nhelp - list commands\n");
	return;
}
struct shelltab_entry shell_table[]={
	{"echo ", 5, shell_echo},
	{"run ", 4, shell_run},
	{"clear", 5, shell_clear},
	{"file ", 5, shell_file},
	{"rf ", 3, shell_rf},
	{"cf ", 3, shell_cf},
	{"cd ", 3, shell_cd},
	{"df ", 3, shell_df},
	{"wf ", 3, shell_wf},
	{"lf ", 3, shell_lf},
	{"color ", 6, shell_color},
	{"help", 4, shell_help},
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
		unsigned int last_time = sys_get_time_ms();
		entry.pfunc(cmd, cmdlen);
		unsigned int elapsed_time = sys_get_time_ms()-last_time;
		printf("executed in %dms\n", elapsed_time);
		return 0;
	}
	return -1;
}
int _start(void){
	sys_clear();
	unsigned int bootdrive = sys_getbootdrive();
	sys_set_console_color(VGA_COLOR_BRIGHT_WHITE, VGA_COLOR_BLACK);
	sys_print("dynamic interactive shell loaded!\n");	
	while (1){
		char input[256] = {0};
		scan(input, sizeof(input)-1, '\n');
		execute_cmd(input);
	}
	return 0;
}
