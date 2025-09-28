#include <stddef.h>
#include "video.h"
#include "kernel.h"
#include "panic.h"
#include "thread.h"
#include "context.h"
#include "stdlib.h"
#include "timer.h"
unsigned int time_ms = 0;
unsigned int xor_state = 0;
struct scheduler_task_info_t scheduler_info = {0};
unsigned int get_time_ms(void){
	return time_ms;
}
void set_pit_frequency(unsigned int freq){
	__asm__ volatile("cli");
	unsigned int div = 1193182/freq;
	outb(0x40, div&0xFF);
	outb(0x40, (div>>8)&0xFF);
	return;
}
void sleep(unsigned int ms){
	unsigned int before = time_ms;
	while ((time_ms-before)<ms){};
	return;
}
void set_multithreading(unsigned int value){
	scheduler_info.multithread_enabled = value;
	return;
}
unsigned int random(unsigned int min, unsigned int max){
	if (!xor_state)
		xor_state = time_ms;
	unsigned int x = xor_state;
	x ^= x <<13;
	x ^= x >> 17;
	x ^= x << 5;
	xor_state = x;
	return (x%(max-min))+min;
}
