#include "video.h"
#include "bootloader.h"
#include "interrupt.h"
#include "stdlib.h"
#include "keyboard.h"
#include "kernel.h"
unsigned char shiftPressed = 0;
unsigned char capsPressed = 0;
void kentry(void){
	struct bootloader_args* blargs = (struct bootloader_args*)0x2000;
	idt_init();
	print("x86 protected mode bootloader\n");
	print("test\n");
	printf("%d %s %c\n", 1234, "toilet", 'a');
	printf("drive number: %p\n", (void*)blargs->bootdrive);
	while (1){};
	return;	
}
void keyboard_interrupt(void){
	if (!(inb(0x64)&1))
		return;
	uint8_t scancode = inb(0x60);
	switch (scancode){
		case 0x2A:
		case 0x36:
			shiftPressed = 1;	
		return;
		case 0xAA:
		case 0xB6:
			shiftPressed = 0;
		return;
	}
	if (scancode>sizeof(scantoascii))
		return;
	char ascii = scantoascii[scancode];
	if (!ascii)
		return;
	if (capsPressed||shiftPressed)
		ascii = toUpper(ascii);
	putchar(ascii);
	return;
}
void outb(uint16_t port, uint8_t value){
	__asm__ volatile("outb %0, %1" : : "a"(value),"Nd"(port));
	return;
}
uint8_t inb(uint16_t port){
	uint8_t data = 0;
	__asm__ volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}
