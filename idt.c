#include <stdint.h>
#include "video.h"
#include "bootloader.h"
#include "interrupt.h"
#include "timer.h"
#include "kernel.h"
__attribute__((aligned(0x10)))
struct idt_entry_t idttable[IDT_MAX_ENTRIES] = {0};
int idt_set_descriptor(uint8_t vector, uint32_t isr, uint8_t flags){
	idttable[vector].isr_low = isr&0xFFFF;
	idttable[vector].kernel_cs = 0x08;
	idttable[vector].attribs = flags;
	idttable[vector].isr_high = isr>>16;
	idttable[vector].reserved = 0;
	return 0;
}
int idt_init(void){
	struct idtr_t idtr = {0};
	idtr.base = (uint32_t)idttable;
	idtr.limit = sizeof(struct idt_entry_t)*IDT_MAX_ENTRIES-1;
	for (uint8_t i = 0;i<IDT_MAX_ENTRIES;i++){
		idt_set_descriptor(i, (uint32_t)default_master_isr, 0x8E);		
	}
	idt_set_descriptor(0x00, (uint32_t)isr0, 0x8E);
	idt_set_descriptor(1, (uint32_t)isr1, 0x8E);
	idt_set_descriptor(2, (uint32_t)isr2, 0x8E);
	idt_set_descriptor(3, (uint32_t)isr3, 0x8E);
	idt_set_descriptor(4, (uint32_t)isr4, 0x8E);
	idt_set_descriptor(5, (uint32_t)isr5, 0x8E);
	idt_set_descriptor(6, (uint32_t)isr6, 0x8E);
	idt_set_descriptor(7, (uint32_t)isr7, 0x8E);
	idt_set_descriptor(8, (uint32_t)isr8, 0x8E);
	idt_set_descriptor(9, (uint32_t)isr9, 0x8E);
	idt_set_descriptor(10, (uint32_t)isr10, 0x8E);
	idt_set_descriptor(11, (uint32_t)isr11, 0x8E);
	idt_set_descriptor(12, (uint32_t)isr12, 0x8E);
	idt_set_descriptor(13, (uint32_t)isr13, 0x8E);
	idt_set_descriptor(14, (uint32_t)isr14, 0x8E);
	idt_set_descriptor(16, (uint32_t)isr16, 0x8E);
	idt_set_descriptor(17, (uint32_t)isr17, 0x8E);
	idt_set_descriptor(18, (uint32_t)isr18, 0x8E);
	idt_set_descriptor(19, (uint32_t)isr19, 0x8E);
	idt_set_descriptor(20, (uint32_t)isr20, 0x8E);
	idt_set_descriptor(21, (uint32_t)isr21, 0x8E);
	idt_set_descriptor(28, (uint32_t)isr28, 0x8E);
	idt_set_descriptor(29, (uint32_t)isr29, 0x8E);
	idt_set_descriptor(30, (uint32_t)isr30, 0x8E);
	idt_set_descriptor(0x20, (uint32_t)timer_isr, 0x8E);
	idt_set_descriptor(0x21, (uint32_t)keyboard_isr, 0x8E);
	unsigned char a1 = inb(0x21);
	unsigned char a2 = inb(0xA1);
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
	set_pit_frequency(1000);
	__asm__ volatile ("lidt %0" : : "m"(idtr));
	__asm__ volatile ("sti");	
	return 0;
}
