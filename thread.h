#ifndef _THREAD
#define _THREAD
enum threadStatus{
	THREAD_INVALID,
	THREAD_FREE,	
	THREAD_INUSE,
	THREAD_SUSPENDED,
};
struct threadstate_x86{
	uint32_t eip;
	uint32_t esp;
	uint32_t ebp;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
}__attribute__((packed));
struct thread_t{
	struct thread_t* blink;
	struct thread_t* flink;
	void* arg;
	struct threadstate_x86 state;
	uint32_t id;
	uint32_t stack_start;
	uint32_t stacksize;
	enum threadStatus status;
}__attribute__((packed));
struct scheduler_task_info_t{
	struct thread_t* last_thread;
	struct thread_t* current_thread;
	uint32_t thread_max_ms;
	uint32_t thread_start;
	uint32_t multithread_enabled;
}__attribute__((packed));
typedef void(*threadfunc)(void* arg);
extern struct thread_t* first_thread;
extern struct scheduler_task_info_t scheduler_info;
extern uint32_t threads_created;
extern uint32_t threads_freed;
extern uint32_t threads_cnt;
struct thread_t* thread_create(uint32_t eip, uint32_t stack_size, void* arg);
int thread_free(struct thread_t* thread);
int switch_task(struct thread_t* ptask);
#endif
