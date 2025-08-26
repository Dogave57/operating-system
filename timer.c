#include <stddef.h>
#include "video.h"
#include "kernel.h"
#include "panic.h"
#include "thread.h"
#include "timer.h"
unsigned int time_ms = 0;
struct scheduler_task_info_t scheduler_info = {0};
__attribute__((cdecl)) void timer_interrupt(uint32_t edi, uint32_t esi, uint32_t ebp, uint32_t esp, uint32_t ebx, uint32_t edx, uint32_t ecx, uint32_t eax, uint32_t eip, uint32_t cs, uint32_t flags){
	time_ms++;
	if (cs!=0x8){
		panic("code segement corrupted!\n");
		return;
	}
	if (!scheduler_info.multithread_enabled||!first_thread){
		return;
	}
	static unsigned int elapsed_time = 0;
	elapsed_time = (unsigned int)((time_ms-scheduler_info.thread_start));
	if (elapsed_time<scheduler_info.thread_max_ms){
		return;	
	}
	if (scheduler_info.current_thread!=(struct thread_t*)0x0){
		scheduler_info.current_thread->state.eip = eip;
		scheduler_info.current_thread->state.eax = eax;
		scheduler_info.current_thread->state.edx = edx;
		scheduler_info.current_thread->state.ebx = ebx;
		scheduler_info.current_thread->state.esp = esp;
		scheduler_info.current_thread->state.ebp = ebp;
		scheduler_info.current_thread->state.esi = esi;
		scheduler_info.current_thread->state.edi = edi;
		printf("eip: %p\n", (void*)scheduler_info.current_thread->state.eip);
		printf("esp: %p\n", (void*)scheduler_info.current_thread->state.esp);
		printf("code segment: %p\n", (void*)cs);
	}
	scheduler_info.thread_start = time_ms;
	scheduler_info.thread_max_ms = 1000;
	if (!threads_cnt){
		panic("no threads to run!\n");
		while(1){};
		return;
	}
	scheduler_info.last_thread = scheduler_info.current_thread;
	if (scheduler_info.current_thread!=NULL){
		scheduler_info.current_thread = scheduler_info.current_thread->flink;
	//	printf("flink: %p\n", (void*)scheduler_info.current_thread->flink);
	}
	if (!scheduler_info.current_thread)
		scheduler_info.current_thread = first_thread;
	if (scheduler_info.current_thread->status!=THREAD_INUSE){
		while (scheduler_info.current_thread->status!=THREAD_INUSE&&scheduler_info.current_thread!=NULL){
			scheduler_info.current_thread = scheduler_info.current_thread->flink;
		}
	}
	if (!scheduler_info.current_thread){
		panic("no threads to run!\n");
		while (1){};
		return;
	}
	if (scheduler_info.last_thread==scheduler_info.current_thread){
		return;
	}
	//	printf("esp: %p\n", (void*)scheduler_info.current_thread->state.esp);
	printf("executing thread: %d with eip: %p\n", scheduler_info.current_thread->id, (void*)scheduler_info.current_thread->state.eip);
	threadfunc func = (threadfunc)scheduler_info.current_thread->state.eip;
	__asm__ volatile("mov %0, %%eax" :: "r"(scheduler_info.current_thread->state.eax));
	__asm__ volatile("mov %0, %%ebx" :: "r"(scheduler_info.current_thread->state.ebx));
	__asm__ volatile("mov %0, %%ecx" :: "r"(scheduler_info.current_thread->state.ecx));
	__asm__ volatile("mov %0, %%edx" :: "r"(scheduler_info.current_thread->state.edx));
	__asm__ volatile("mov %0, %%edi" :: "r"(scheduler_info.current_thread->state.edi));
	__asm__ volatile("mov %0, %%esi" :: "r"(scheduler_info.current_thread->state.esi));
/*	__asm__ volatile("mov %0, %%di" :: "r"(scheduler_info.current_thread->state.di));
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
*/
	static void* thread_arg = NULL;
	scheduler_info.current_thread->arg;
	//printf("running thread with eip: %p\n", (void*)scheduler_info.current_thread->state.eip);
//	__asm__ volatile("mov %0, %%esp" :: "r"(scheduler_info.current_thread->state.esp));
//	__asm__ volatile("mov %0, %%ebp" :: "r"(scheduler_info.current_thread->state.ebp));
	outb(0x20,0x20);
//	__asm__ volatile("mov %0, %%ebp" :: "r"(scheduler_info.current_thread->state.ebp));
	__asm__ volatile("mov %0, %%esp" :: "r"(scheduler_info.current_thread->state.esp));
	__asm__ volatile("sti");
	func(thread_arg);
//	printf("thread returned! Terminated thread...\n");
	thread_free(scheduler_info.current_thread);
	scheduler_info.current_thread = scheduler_info.current_thread->flink;
	scheduler_info.thread_max_ms = 0;
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
