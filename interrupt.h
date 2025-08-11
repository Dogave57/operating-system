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
void keyboard_isr(void);
void keyboard_interrupt(void);
int idt_set_descriptor(uint8_t vector, uint32_t isr, uint8_t flags);
int idt_init(void);
#endif
