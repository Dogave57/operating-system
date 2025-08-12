#include "video.h"
#include "kernel.h"
#include "panic.h"
#include "timer.h"
unsigned int time_ms = 0;
void timer_interrupt(void){
	time_ms++;
	return;
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
