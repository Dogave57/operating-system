#include "libsys.h"
#include "stdlib.h"
struct text_link{
	unsigned int flink;
	unsigned int blink;
};
static char* filedata = (char*)0x0;
static unsigned int filesize = 0;
static struct text_link* plinks = (struct text_link*)0x0;
static unsigned int cursor_position = 0;
int move_cursor(unsigned int new_position);
int move_cursor(unsigned int new_position){
	struct vector2 pos = {0};
	struct vector2 size = {8,8};
	pos.x = (cursor_position*8)%320;
	pos.y = (cursor_position*8)/320;
	char currentchar = 0;
	if (cursor_position<filesize)
		currentchar = filedata[cursor_position];
	sys_draw_rect(pos, size, VGA_COLOR_BLACK);
	if (currentchar)
		sys_writechar(cursor_position*8, currentchar, VGA_COLOR_BRIGHT_WHITE, VGA_COLOR_BLACK);
	pos.x = (new_position*8)%320;
	pos.y = (new_position*8)/320;
	sys_draw_rect(pos, size, VGA_COLOR_BRIGHT_WHITE);
	cursor_position = new_position;
	return 0;
}
int _start(char** argp, unsigned int argc){
	if (!argp)
		return -1;
	if (argc<1){
		return -1;
	}
	char* filename = argp[0];
	sys_clear();
	printf("loading file %s\n", filename);
	printf("Welcome to text edit\n");
	unsigned int bootdrive = sys_getbootdrive();
	struct file* pfile = sys_openfile(bootdrive, filename);
	if (!pfile){
		printf("failed to open file\n");
		return -1;
	}
	filesize = sys_getfilesize(pfile);
	char* filedata = (char*)sys_kmalloc(filesize);
	if (!filedata){
		printf("failed to allocate memory for file data\n");
		sys_closefile(pfile);
		return -1;
	}
	memset((void*)filedata, 0, filesize);
	if (sys_readfile(pfile, filedata)!=0){
		printf("failed to read file into memory\n");
		sys_closefile(pfile);
		sys_kfree((void*)filedata);
		return -1;
	}
	sys_closefile(pfile);
	unsigned int linksize = filesize*sizeof(struct text_link);
	struct text_link* plinks = (struct text_link*)sys_kmalloc(linksize);
	if (!plinks){
		printf("failed to allocate memory for file links\n");
		sys_kfree((void*)filedata);
		return -1;
	}
	memset((void*)plinks, 0, linksize);
	sys_clear();
	sys_print(filedata);
	cursor_position = 0;
	unsigned int cooldown = 0;
	unsigned int charsper_row = (320/8);
	unsigned int charsper_col = (200/8);
	while (1){
		if (sys_keypressed(KEY_ESC))
			break;
		unsigned int time_ms = sys_get_time_ms();
		unsigned int lastchar = sys_getchar();
		if (lastchar=='\b'){
			if (cursor_position){
			sys_writechar(cursor_position, ' ', VGA_COLOR_BRIGHT_WHITE, VGA_COLOR_BLACK);
			filedata[cursor_position] = ' ';
			move_cursor(cursor_position-1);	
			}
			
		}
		if ((time_ms-cooldown)<50&&cooldown)
			continue;
		cooldown = time_ms;
		unsigned int old_position = cursor_position;
		unsigned int arrow = 255;
		unsigned int pressed = 0;
		if (sys_keypressed(KEY_RARROW)){
			move_cursor(cursor_position+1);
			pressed = 1;
			continue;
		}
		if (sys_keypressed(KEY_LARROW)&&!pressed){
			if (cursor_position)
				move_cursor(cursor_position-1);
			pressed = 1;
			continue;
		}
		if (sys_keypressed(KEY_DARROW)&&!pressed){
			move_cursor(cursor_position+(charsper_row*8));
			pressed = 1;
			continue;
		}
		if (sys_keypressed(KEY_UARROW)&&!pressed){
			if (cursor_position<charsper_row)
				continue;
			move_cursor(cursor_position-(charsper_row*8));
			pressed = 1;
			continue;
		}
	}
	sys_kfree((void*)filedata);
	sys_kfree((void*)plinks);
	sys_clear();
	return 0;
}
