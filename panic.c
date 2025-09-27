#include "video.h"
#include "speaker.h"
#include "panic.h"
void panic(const char* reason){
	__asm__ volatile("cli");
	if (!reason)
		reason = "no reason\n";
	vga_set_bg(VGA_COLOR_BLUE);
	clear();
	play_sound(1000);
	print("KERNEL PANIC\n");
	print(reason);
	__asm__ volatile("hlt");
	return;
}
