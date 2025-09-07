#include "video.h"
#include "panic.h"
void panic(const char* reason){
	__asm__ volatile("cli");
	if (!reason)
		reason = "no reason\n";
	clear();
	print("KERNEL PANIC\n");
	print(reason);
	__asm__ volatile("hlt");
	return;
}
