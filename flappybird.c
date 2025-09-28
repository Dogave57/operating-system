#include "stdlib.h"
#include "panic.h"
#include "libsys.h"
#include "vga.h"
#define BASE_GRAVITY 1
#define NPIPES 1
static int gravity = BASE_GRAVITY;
static struct pipe* ppipelist = (struct pipe*)0x0;
static struct pipe* lastpipe = (struct pipe*)0x0;
struct pipe{
	struct object_rect* top;
	struct object_rect* bottom;
	struct pipe* flink;
	struct pipe* blink;	
};
int reset_pipe(struct pipe* ppipe);
int init_pipes(unsigned int pipecnt);
int deinit_pipes(void);
int move_pipes(void);
int reset_pipes(void);
int reset_pipe(struct pipe* ppipe){
	if (!ppipe)
		return -1;
	struct vector2 pos = {0};
	struct vector2 size = {0};
	pos.x = 320-30;
	pos.y = 0;
	size.x = 30;
	size.y = sys_random(30, 70);
	ppipe->top->position = pos;
	ppipe->top->size = size;
	pos.x = 320-30;
	pos.y = size.y+70+sys_random(10, 20);
	size.x = 30;
	size.y = 200-pos.y;
	ppipe->bottom->position = pos;
	ppipe->bottom->size = size;
	return 0;
}
int init_pipes(unsigned int pipecnt){
	if (!pipecnt)
		return -1;
	for (unsigned int i = 0;i<pipecnt;i++){
		struct pipe* newpipe = (struct pipe*)sys_kmalloc(sizeof(struct pipe));
		if (!newpipe)
			return -1;
		struct vector2 pos = {0};
		struct vector2 size = {0};
		newpipe->top = sys_add_rect(pos, size, FRAME_FG);
		if (!newpipe->top){
			printf("failed to create new pipe object\n");
			sys_kfree((void*)newpipe);
			deinit_pipes();
			return -1;
		}
		newpipe->bottom = sys_add_rect(pos, size, FRAME_FG);
		if (!newpipe->bottom){
			printf("failed to create new pipe object\n");
			sys_kfree((void*)newpipe);
			deinit_pipes();
			return -1;
		}
		newpipe->top->color = VGA_COLOR_GREEN;
		newpipe->bottom->color = VGA_COLOR_GREEN;
		reset_pipe(newpipe);
		if (!ppipelist){
			ppipelist = newpipe;
			lastpipe = newpipe;
			continue;
		}
		if (!lastpipe){
			lastpipe = newpipe;
			continue;
		}
		lastpipe->flink = newpipe;
		newpipe->blink = lastpipe;
		lastpipe = newpipe;
	}	
	return 0;
}
int deinit_pipes(void){
	if (!ppipelist)
		return -1;
	struct pipe* pipe = ppipelist;
	while (pipe){
		struct pipe* flink = pipe->flink;
		if (pipe->top)
			sys_remove_object((struct object*)pipe->top);
		if (pipe->bottom)
			sys_remove_object((struct object*)pipe->bottom);
		sys_kfree((void*)pipe);
		pipe = flink;
	}
	return 0;
}
int move_pipes(void){
	if (!ppipelist)
		return -1;
	struct pipe* pipe = ppipelist;
	while (pipe){
		struct pipe* flink = pipe->flink;
		pipe->bottom->position.x--;
		pipe->top->position.x--;
		if (pipe->bottom->position.x>-1){
			pipe = flink;
			continue;
		}
		reset_pipe(pipe);
		pipe = flink;
	}
	return 0;
}
int reset_pipes(void){
	if (!ppipelist)
		return -1;
	struct pipe* pipe = ppipelist;
	while (pipe){
		struct pipe* flink = pipe->flink;
		reset_pipe(pipe);
		pipe = flink;
	}
}
int _start(char** argp, unsigned int argc){
	sys_set_bg(VGA_COLOR_LIGHT_BLUE);
	sys_clear();
	printf("Destroy this text\n");
	if (sys_init_objects()!=0){
		printf("failed to initialize objects\n");
		return -1;
	}
	struct vector2 pos = {50,100};
	struct vector2 size = {30,30};
	struct object_rect* bird = sys_add_rect(pos, size, FRAME_MIDDLE);
	if (!bird){
		printf("failed to create bird\n");
		return -1;
	}
	bird->color = VGA_COLOR_RED;
	sys_render_objects();
	if (init_pipes(NPIPES)!=0){
		printf("failed to initialize pipes\n");
		if (sys_deinit_objects()!=0)
			printf("failed to deinitialize pipes\n");
		return -1;
	}
	unsigned int jmpCooldown = 0;
	unsigned int gravityCooldown = 0;
	while (1){
		unsigned int time_ms = sys_get_time_ms();
		if (sys_keypressed(KEY_ESC)&&sys_keypressed('x')){
			if (deinit_pipes()!=0)
				printf("failed to deinitialize pipes\n");
			if (sys_deinit_objects()!=0)
				printf("failed to deinitialize objects\n");
			sys_set_bg(VGA_COLOR_BLACK);
			sys_clear();
			sys_getchar();
			return 0;
		}
		if (sys_keypressed(' ')&&gravity==BASE_GRAVITY&&(!jmpCooldown||((time_ms-jmpCooldown)>200))){
			gravity = BASE_GRAVITY*-6;
			jmpCooldown = time_ms;
		}
		if (gravity<BASE_GRAVITY&&(time_ms-jmpCooldown)>50)
			gravity++;
		if (!gravityCooldown||(time_ms-gravityCooldown)>5){
			bird->position.y+=gravity;
			gravityCooldown = time_ms;
			move_pipes();
		}
		if (bird->position.y<0||bird->position.y>200){
			bird->position.x = pos.x;
			bird->position.y = pos.y;
			gravity = BASE_GRAVITY;
			printf("You lost!\n");
			sys_beep(1000,1000);
			sys_getchar();
			printf("Press any key to continue...\n");
			while (!sys_getchar()){};
			sys_getchar();
			reset_pipes();
			sys_clear();
		}
		time_ms = sys_get_time_ms();
		sys_render_objects();
		unsigned int elapsed = sys_get_time_ms()-time_ms;
		if (!elapsed)
			elapsed = 1;
		unsigned int fps = 1000/elapsed;
	}
	if (deinit_pipes()!=0)
		printf("failed to deinitialize pipes\n");
	printf("deinitializing objects\n");
	if (sys_deinit_objects()!=0)
		printf("failed to deinitialize objects\n");
	return 0;
}
