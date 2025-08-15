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
	uint32_t edi;
	uint32_t esi;
	uint16_t di;
	uint16_t si;
	uint16_t ax;
	uint16_t cx;
	uint16_t dx;
	uint8_t ah;
	uint8_t al;
	uint8_t bh;
	uint8_t bl;
	uint8_t ch;
	uint8_t cl;
	uint8_t dh;
	uint8_t dl;
};
struct thread_t{
	uint32_t id;
	uint32_t stacksize;
	struct threadstate_x86 state;
	struct thread_t* blink;
	struct thread_t* flink;
	enum threadStatus status;
	void* arg;
};
struct scheduler_task_info_t{
	struct thread_t* last_used_thread;
	struct thread_t* current_thread;
	uint32_t thread_max_ms;
	uint32_t thread_start;
	uint32_t esp;
	uint32_t ebp;
	uint32_t multithread_enabled;
};
typedef void(*threadfunc)(void* arg);
extern struct thread_t* first_thread;
extern struct scheduler_task_info_t scheduler_info;
extern uint32_t threads_created;
extern uint32_t threads_freed;
extern uint32_t threads_cnt;
struct thread_t* thread_create(uint32_t eip, uint32_t stack_size, void* arg);
int thread_free(struct thread_t* thread);
#endif
