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
