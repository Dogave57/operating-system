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
	struct thread_t* newthread = (struct thread_t*)NULL;
	struct thread_t* link = first_thread;
	while (link!=NULL){
		if (link->status!=THREAD_FREE){
			link=link->flink;
			continue;
		}	
		newthread = link;
		newthread->flink = NULL;
		newthread->blink = NULL;
		break;
	}
	if (!newthread){
		newthread = (struct thread_t*)kmalloc(sizeof(struct thread_t));
		if (!newthread){
			print("failed to allocate memory for new thread\n");
			return NULL;
		}
		memset((void*)newthread, 0, sizeof(struct thread_t));
		newthread->id = threads_created;
		threads_created++;
	}
	last_thread->flink = newthread;
	newthread->blink = last_thread;
	last_thread = newthread;
	last_thread->flink = first_thread;	
	last_thread = newthread;
	if (!first_thread)
		first_thread = newthread;
	newthread->state.esp = (uint32_t)kmalloc(stack_size);
	if (!newthread->state.esp){
		print("failed to allocate stack for thread\n");
		newthread->status = THREAD_FREE;
		return NULL;
	}
	newthread->stacksize = stack_size;
	newthread->state.eip = eip;
	if (newthread->status==THREAD_FREE)
		threads_freed--;
	newthread->status = THREAD_INUSE;
	threads_count++;
	return newthread;
}
int thread_free(struct thread_t* thread){
	if (!thread)
		return -1;
	if (thread->state.esp)
		kfree((void*)thread->state.esp);
	thread->status = THREAD_FREE;
	threads_count--;
	threads_freed++;
	return 0;
}
