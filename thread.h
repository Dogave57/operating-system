#ifndef _THREAD
#define _THREAD
struct threadstate_x86{
	uint32_t eip;
	uint32_t esp;
	uint32_t ebp;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t edi;
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
	struct threadstate_x86 state;
	struct thread_t* blink;
	struct thread_t* flink;
};
extern struct thread_t* first_thread;
extern struct thread_t* last_thread;
extern uint32_t threads_created;
extern uint32_t threads_freed;
extern uint32_t threads_cnt;
struct thread_t* thread_create(uint32_t eip, uint32_t stack_size);
int thread_free(struct thread_t* thread);
#endif
