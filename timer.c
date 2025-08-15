#include <stddef.h>
#include "video.h"
#include "kernel.h"
#include "panic.h"
#include "thread.h"
#include "timer.h"
unsigned int time_ms = 0;
struct thread_t* current_thread = (struct thread_t*)NULL;
struct thread_t* last_used_thread = (struct thread_t*)NULL;
struct scheduler_task_info_t scheduler_info = {0};
__attribute__((cdecl)) void timer_interrupt(uint32_t eip){
	time_ms++;
	if (!scheduler_info.multithread_enabled||!first_thread)
		return;
	if (scheduler_info.current_thread!=NULL){
//		__asm__ volatile("mov %%eip, %0" : "=r"(scheduler_info.current_thread->state.eip));
		__asm__ volatile("mov %%esp, %0" : "=r"(scheduler_info.current_thread->state.esp));
		__asm__ volatile("mov %%ebp, %0" : "=r"(scheduler_info.current_thread->state.ebp));
		__asm__ volatile("mov %%eax, %0" : "=r"(scheduler_info.current_thread->state.eax));
		__asm__ volatile("mov %%ebx, %0" : "=r"(scheduler_info.current_thread->state.ebx));
		__asm__ volatile("mov %%ecx, %0" : "=r"(scheduler_info.current_thread->state.ecx));
		__asm__ volatile("mov %%edx, %0" : "=r"(scheduler_info.current_thread->state.edx));
		__asm__ volatile("mov %%edi, %0" : "=r"(scheduler_info.current_thread->state.edi));
		__asm__ volatile("mov %%esi, %0" : "=r"(scheduler_info.current_thread->state.esi));
		__asm__ volatile("mov %%di, %0" : "=r"(scheduler_info.current_thread->state.di));
		__asm__ volatile("mov %%si, %0" : "=r"(scheduler_info.current_thread->state.si));
		__asm__ volatile("mov %%ax, %0" : "=r"(scheduler_info.current_thread->state.ax));
		__asm__ volatile("mov %%cx, %0" : "=r"(scheduler_info.current_thread->state.cx));
		__asm__ volatile("mov %%dx, %0" : "=r"(scheduler_info.current_thread->state.dx));
		__asm__ volatile("mov %%ah, %0" : "=r"(scheduler_info.current_thread->state.ah));
		__asm__ volatile("mov %%al, %0" : "=r"(scheduler_info.current_thread->state.al));
		__asm__ volatile("mov %%bh, %0" : "=r"(scheduler_info.current_thread->state.bh));
		__asm__ volatile("mov %%bl, %0" : "=r"(scheduler_info.current_thread->state.bl));
		__asm__ volatile("mov %%ch, %0" : "=r"(scheduler_info.current_thread->state.ch));
		__asm__ volatile("mov %%cl, %0" : "=r"(scheduler_info.current_thread->state.cl));
		__asm__ volatile("mov %%dh, %0" : "=r"(scheduler_info.current_thread->state.dh));
		__asm__ volatile("mov %%dl, %0" : "=r"(scheduler_info.current_thread->state.dl));
		scheduler_info.current_thread->state.eip = eip;
		printf("eip: %p\n", (void*)eip);
	}
	if ((time_ms-scheduler_info.thread_start)<scheduler_info.thread_max_ms)
		return;
	scheduler_info.thread_start = time_ms;
	scheduler_info.thread_max_ms = 1000;
	if (!threads_cnt){
		panic("no threads to run!\n");
		while(1){};
		return;
	}
	if (scheduler_info.current_thread!=NULL){
		scheduler_info.current_thread = scheduler_info.current_thread->flink;
		printf("flink: %p\n", (void*)scheduler_info.current_thread->flink);
	}
	if (!scheduler_info.current_thread)
		scheduler_info.current_thread = first_thread;
	if (scheduler_info.current_thread->status!=THREAD_INUSE){
		while (scheduler_info.current_thread->status!=THREAD_INUSE&&scheduler_info.current_thread!=NULL){
			scheduler_info.current_thread = scheduler_info.current_thread->flink;
		}
	}
//	printf("esp: %p\n", (void*)scheduler_info.current_thread->state.esp);
//	printf("executing thread: %d with eip: %p\n", scheduler_info.current_thread->id, (void*)scheduler_info.current_thread->state.eip);
	threadfunc func = (threadfunc)scheduler_info.current_thread->state.eip;
//	__asm__ volatile("mov %0, %%esp" :: "r"(scheduler_info.current_thread->state.esp));
//	__asm__ volatile("mov %0, %%ebp" :: "r"(scheduler_info.current_thread->state.ebp));
	__asm__ volatile("mov %0, %%eax" :: "r"(scheduler_info.current_thread->state.eax));
	__asm__ volatile("mov %0, %%ebx" :: "r"(scheduler_info.current_thread->state.ebx));
	__asm__ volatile("mov %0, %%ecx" :: "r"(scheduler_info.current_thread->state.ecx));
	__asm__ volatile("mov %0, %%edx" :: "r"(scheduler_info.current_thread->state.edx));
	__asm__ volatile("mov %0, %%edi" :: "r"(scheduler_info.current_thread->state.edi));
	__asm__ volatile("mov %0, %%esi" :: "r"(scheduler_info.current_thread->state.esi));
	__asm__ volatile("mov %0, %%di" :: "r"(scheduler_info.current_thread->state.di));
	__asm__ volatile("mov %0, %%si" :: "r"(scheduler_info.current_thread->state.si));
	__asm__ volatile("mov %0, %%ax" :: "r"(scheduler_info.current_thread->state.ax));
	__asm__ volatile("mov %0, %%cx" :: "r"(scheduler_info.current_thread->state.cx));
	__asm__ volatile("mov %0, %%dx" :: "r"(scheduler_info.current_thread->state.dx));
	__asm__ volatile("mov %0, %%ah" :: "r"(scheduler_info.current_thread->state.ah));
	__asm__ volatile("mov %0, %%al" :: "r"(scheduler_info.current_thread->state.al));
	__asm__ volatile("mov %0, %%bh" :: "r"(scheduler_info.current_thread->state.bh));
	__asm__ volatile("mov %0, %%bl" :: "r"(scheduler_info.current_thread->state.bl));
	__asm__ volatile("mov %0, %%ch" :: "r"(scheduler_info.current_thread->state.ch));
	__asm__ volatile("mov %0, %%cl" :: "r"(scheduler_info.current_thread->state.cl));
	__asm__ volatile("mov %0, %%dh" :: "r"(scheduler_info.current_thread->state.dh));
	__asm__ volatile("mov %0, %%dl" :: "r"(scheduler_info.current_thread->state.dl));
	outb(0x20,0x20);
	void* thread_arg = scheduler_info.current_thread->arg;
	__asm__ volatile("sti");
	func(thread_arg);
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
	scheduler_info.multithread_enabled = value;
	return;
}
