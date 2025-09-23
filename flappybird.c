#include "stdlib.h"
#include "libsys.h"
int _start(void){
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
	while (1){
		sys_render_objects();
		unsigned char ch = sys_getchar();
		if (!ch)
			continue;
		bird->position.y--;	
		bird->color = sys_random(12,15);
	}
	printf("deinitializing objects\n");
	if (sys_deinit_objects()!=0)
		printf("failed to deinitialize objects\n");
	return 0;
}
