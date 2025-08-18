#include "stdlib.h"
#include "video.h"
#include "kernel.h"
#include "timer.h"
#include "thread.h"
struct thread_t* first_thread = (struct thread_t*)NULL;
struct thread_t* last_thread = (struct thread_t*)NULL;
uint32_t threads_created = 0;
uint32_t threads_freed = 0;
uint32_t threads_cnt = 0;
struct thread_t* thread_create(uint32_t eip, uint32_t stack_size, void* arg){
	struct thread_t* newthread = (struct thread_t*)NULL;
	struct thread_t* link = first_thread;
	while (newthread&&newthread->status!=THREAD_FREE){
		newthread = newthread->flink;
		memset((void*)newthread,0 , sizeof(struct thread_t));	
		newthread->status = THREAD_FREE;
	}
	if (!newthread){
		newthread = (struct thread_t*)kmalloc(sizeof(struct thread_t));
		if (!newthread){
			print("failed to allocate memory for new thread\n");
			return NULL;
		}
		memset((void*)newthread, 0, sizeof(struct thread_t));
	}
	memset((void*)newthread, 0, sizeof(struct thread_t));
	newthread->stack_start = (uint32_t)kmalloc(stack_size);
	if (!newthread->stack_start){
		print("failed to allocate memory for stack\n");
		kfree((void*)newthread);
		return NULL;	
	}
	newthread->state.esp = newthread->stack_start+stack_size;
	newthread->state.ebp = newthread->state.esp;
	newthread->blink = last_thread;
	if (last_thread)
		last_thread->flink = newthread;
	newthread->flink = first_thread;
	last_thread = newthread;
	if (!first_thread)
		first_thread = newthread;
	if (newthread->status!=THREAD_FREE)
		threads_created++;
	newthread->state.eip = eip;
	newthread->stacksize = stack_size;
	newthread->status = THREAD_INUSE;
	newthread->id = threads_created;
	newthread->arg = arg;
	threads_cnt++;
	print("done creating thread\n");
	return newthread;
}
int thread_free(struct thread_t* thread){
	if (!thread)
		return -1;
	if (thread->stack_start)
		kfree((void*)thread->stack_start);
	thread->status = THREAD_FREE;
	threads_cnt--;
	threads_freed++;
	return 0;
}
