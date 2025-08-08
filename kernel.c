#include "video.h"
#include "bootloader.h"
#include "interrupt.h"
#include "stdlib.h"
#include "keyboard.h"
#include "commands.h"
#include "cursor.h"
#include "panic.h"
#include "kernel.h"
unsigned char shiftPressed = 0;
unsigned char capsPressed = 0;
char cmdline[256] = {0};
size_t cmdlen = 0;
void kentry(void){
	struct bootloader_args* blargs = (struct bootloader_args*)0x2000;
	if (idt_init()!=0){
		panic("failed to load idt\n");
		__asm__ volatile("hlt");
		return;
	}
	cursor_enable(0x00, 15);
	printf("cpu vendor: %s\n", blargs->vendorid);
	printf("cpu product name: %s\n", blargs->productname);
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
	if (cmdlen<255){
		cmdline[cmdlen] = ascii;
		cmdlen++;
	}
	switch(ascii){
		case '\n':
			cmdline[cmdlen] = 0;
			executecmd(cmdline, cmdlen);
			cmdlen = 0;
			cmdline[0] = 0;
			break;
	}
	return;
}
void reboot(void){
	__asm__ volatile("cli");
	uint8_t good = 0x02;
	while (good&0x2)
		good = inb(0x64);
	outb(0x64, 0xFE);
	__asm__ volatile("hlt");
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
