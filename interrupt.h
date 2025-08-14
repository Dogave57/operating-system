#ifndef _IDT
#define _IDT
#include <stdint.h>
#define IDT_MAX_ENTRIES 255
__attribute__((aligned(0x10)))
struct idt_entry_t{
	uint16_t isr_low;
	uint16_t kernel_cs;
	uint8_t reserved;
	uint8_t attribs;
	uint16_t isr_high;
}__attribute__((packed));
struct idtr_t{
	uint16_t limit;
	uint32_t base;

}__attribute__((packed));
extern struct idt_entry_t idttable[IDT_MAX_ENTRIES];
void default_master_isr(void);
void default_slave_isr(void);
__attribute__((cdecl)) void timer_isr(uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esp, uint32_t ebp, uint32_t esi, uint32_t edi);
void keyboard_isr(void);
void keyboard_interrupt(void);
int idt_set_descriptor(uint8_t vector, uint32_t isr, uint8_t flags);
int idt_init(void);
#endif
