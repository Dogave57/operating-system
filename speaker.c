#include "video.h"
#include "timer.h"
#include "kernel.h"
int play_sound(unsigned int frequency){
	if (!frequency)
		frequency = 1;
	unsigned int div = 1193180 / frequency;
	unsigned char tmp = 0;
	outb(0x43, 0xB6);
	outb(0x42, (unsigned char)div);
	outb(0x42, (unsigned char)(div>>8));
	tmp = inb(0x61);
	if (tmp!=(tmp|3))
		outb(0x61, tmp | 3);
	return 0;
}
int stop_sound(void){
	unsigned char tmp = inb(0x61)&0xFC;
	outb(0x61, tmp);
	return 0;
}
int beep(unsigned int frequency, unsigned int duration){
	play_sound(frequency);
	sleep(duration);
	stop_sound();
	return 0;
}
