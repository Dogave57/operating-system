#ifndef _TIMER
#define _TIMER
extern unsigned int time_ms;
void set_pit_frequency(unsigned int freq);
void sleep(unsigned int ms);
void set_multithreading(unsigned int value);
#endif
