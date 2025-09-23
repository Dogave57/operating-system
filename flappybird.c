#include "stdlib.h"
#include "libsys.h"
#include "vga.h"
int _start(void){
	sys_set_bg(VGA_COLOR_LIGHT_BLUE);
	sys_clear();
	printf("Welcome to flappy bird\n");
	if (sys_init_objects()!=0){
		printf("failed to initialize objects\n");
		return -1;
	}
	struct vector2 pos = {170,100};
	struct vector2 size = {50,50};
	struct object* bird = sys_add_object(pos, size, OBJ_RECT);
	if (!bird){
		printf("failed to create bird\n");
		return -1;
	}
	bird->color = VGA_COLOR_RED;
	sys_render_objects();
	while (1){
		sys_sleep(5);
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
