#ifndef _TIMER
#define _TIMER
extern unsigned int time_ms;
void set_pit_frequency(unsigned int freq);
void sleep(unsigned int ms);
unsigned int get_time_ms(void);
void set_multithreading(unsigned int value);
unsigned int random(unsigned int min, unsigned int max);
#endif
