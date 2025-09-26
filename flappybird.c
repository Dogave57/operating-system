#include "stdlib.h"
#include "libsys.h"
#include "vga.h"
int _start(void){
	sys_set_bg(VGA_COLOR_LIGHT_BLUE);
	sys_clear();
	printf("Destroy this text\n");
	if (sys_init_objects()!=0){
		printf("failed to initialize objects\n");
		return -1;
	}
	struct vector2 pos = {170,100};
	struct vector2 size = {50,50};
	struct object_rect* bird = sys_add_rect(pos, size, FRAME_FG);
	if (!bird){
		printf("failed to create bird\n");
		return -1;
	}
	bird->color = VGA_COLOR_RED;
	pos.x = 100;
	pos.y = 20;
	size.x = 75;
	size.y = 50;
	static char* text = "Welcome to flappy bird!\ntest\ntest2";
	struct object_text* welcome = sys_add_text(pos, text, FRAME_MIDDLE);
	if (!welcome){
		printf("failed to create welcome text\n");
		return -1;
	}
	sys_beep(10, 100000);
	sys_render_objects();
	while (1){
		sys_sleep(5);
		if (sys_keypressed(KEY_ESC)&&sys_keypressed('x')){
			if (sys_deinit_objects()!=0)
				printf("failed to deinitialize objects\n");
			sys_set_bg(VGA_COLOR_BLACK);
			sys_clear();
			sys_getchar();
			return 0;
		}
		if (!sys_keypressed('w')&&!sys_keypressed('a')&&!sys_keypressed('s')&&!sys_keypressed('d')&&!sys_keypressed(' ')&&!sys_keypressed('\b'))
			continue;
		if (sys_keypressed('w'))
			bird->position.y--;		
		if (sys_keypressed('a'))
			bird->position.x--;
		if (sys_keypressed('s'))
			bird->position.y++;
		if (sys_keypressed('d'))
			bird->position.x++;
		if (sys_keypressed(' ')){
			bird->size.x++;
			bird->size.y++;
		}
		if (sys_keypressed('\b')){
			bird->size.x--;
			bird->size.y--;
		}
		sys_render_objects();
	}
	printf("deinitializing objects\n");
	if (sys_deinit_objects()!=0)
		printf("failed to deinitialize objects\n");
	return 0;
}
