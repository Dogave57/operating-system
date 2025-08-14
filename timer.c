#include <stddef.h>
#include "video.h"
#include "kernel.h"
#include "panic.h"
#include "thread.h"
#include "timer.h"
unsigned int time_ms = 0;
unsigned int thread_start = 0;
unsigned int thread_max_ms = 5;
unsigned int multithread_enabled = 0;
struct thread_t* current_thread = (struct thread_t*)NULL;
__attribute__((cdecl)) void timer_interrupt(uint32_t eip){
	time_ms++;
	if (!multithread_enabled||!first_thread)
		return;
	if ((time_ms-thread_start)<thread_max_ms&&thread_start!=0)
		return;
	thread_start = time_ms;
	if (current_thread){
		current_thread = current_thread->flink;
		while (current_thread->status!=THREAD_FREE&&current_thread)
			current_thread=current_thread->flink;
	}
	if (!current_thread)
		current_thread = first_thread;
	if (current_thread==NULL){
		printf("no threads to execute\n");
		return;
	}
	__asm__ volatile("sti");
	if (thread_start==time_ms){
		__asm__ volatile("mov %0, %%esp" :: "r"(current_thread->state.esp));
	}else
		current_thread->state.eip = eip;
	threadfunc func = (threadfunc)current_thread->state.eip;
	func((void*)0x0);
	printf("returned from thread! Terminating thread\n");
	thread_free(current_thread);
	current_thread = NULL;
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
void set_multithreading(unsigned int value){
	multithread_enabled = value;
	return;
}
