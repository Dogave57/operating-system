#include "stdlib.h"
#include "video.h"
#include "kernel.h"
#include "timer.h"
#include "thread.h"
struct thread_t* first_thread = (struct thread_t*)NULL;
struct thread_t* last_thread = (struct thread_t*)NULL;
uint32_t threads_created = 0;
uint32_t threads_freed = 0;
uint32_t threads_count = 0;
struct thread_t* thread_create(uint32_t eip, uint32_t stack_size){
	struct thread_t* newthread = (struct thread_t*)kmalloc(sizeof(struct thread_t));
	if (!newthread){
		printf("failed to allocate memory for thread\n");
		return NULL;
	}
	newthread->id = threads_created;
	memset((void*)&newthread->state, 0, sizeof(newthread->state));
	newthread->state.eip = eip;
	newthread->state.esp = (uint32_t)kmalloc(stack_size);
	if (!newthread->state.esp){
		printf("failed to allocate threads stack!\n");
		kfree((void*)newthread);
		return NULL;
	}
	if (last_thread){
		newthread->blink = last_thread;
		last_thread->flink = newthread;
		last_thread = newthread;
	}else{
		first_thread = newthread;
		last_thread = newthread;
	}
	threads_created++;
	threads_count++;
	return newthread;
}
int thread_free(struct thread_t* thread){
	if (!thread)
		return -1;
	if (thread->blink)
		thread->blink->flink = thread->flink;
	if (thread->flink)
		thread->flink->blink = thread->blink;
	if (thread==last_thread)
		last_thread = last_thread->blink;
	if (thread->state.esp)
		kfree((void*)thread->state.esp);
	if (thread)
		kfree((void*)thread);
	threads_count--;
	threads_freed++;
	return 0;
}
