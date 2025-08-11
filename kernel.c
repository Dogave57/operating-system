#include "video.h"
#include "bootloader.h"
#include "interrupt.h"
#include "stdlib.h"
#include "keyboard.h"
#include "commands.h"
#include "cursor.h"
#include "panic.h"
#include "memory.h"
#include "filesystem.h"
#include "kernel.h"
unsigned char shiftPressed = 0;
unsigned char capsPressed = 0;
char cmdline[256] = {0};
size_t cmdlen = 0;
void kentry(void){
	struct bootloader_args* blargs = (struct bootloader_args*)0x2000;
	unsigned int avalibleMemory = 0;
	unsigned int bootdrive = getbootdrive();
	if (idt_init()!=0){
		panic("failed to load idt\n");
		__asm__ volatile("hlt");
		return;
	}
	cursor_enable(0x00, 15);
	printf("cpu vendor: %s\n", blargs->vendorid);
	printf("cpu product name: %s\n", blargs->productname);
	printf("boot drive: %d\n", bootdrive);
	for (unsigned int i = 0;i<blargs->memorymap_entries;i++){
		struct memorymap_entry entry = blargs->memorymap[i];
		if (entry.type!=5)
			continue;
		printf("corrupted memory at %p with size %d. Consider replacing your ram\n", (void*)entry.baselow, (int)entry.sizelow);
	}
	avalibleMemory = getAvalibleMemory();
	if (avalibleMemory<64000){
		panic("this operating system needs atleast 64kb of memory to run!\n");
		return;
	}
	printf("%dkb of memory avalible\n", ((int)avalibleMemory/1000));
	for (unsigned int i = 0;i<255;i++){
		for (unsigned int s = 0;s<32;s++){
			unsigned int bitmask = 0x00000000|(i<<16)|(s<<11)|(1<<31);
			outl(0xCF8, bitmask);
			outb(0x0,0x0);
			unsigned int ret = inl(0xCFC);
			if (ret==0xFFFFFFFF)
				continue;
//			printf("device found at pci bus %d device %d\n", i, s);
//			printf("vendor id: %p\n", (void*)(ret&0xFFFF));
//			printf("device id: %p\n", (void*)((ret>>16)&0xFFFF));
		}
	}
	uint16_t* buf = (uint16_t*)100000;
	unsigned int sectorcnt = 1;
	int status = read_sector(bootdrive, 0, sectorcnt, buf);
	if (!status)
		print("successfully read drive\n");
	else
		print("failed to read from drive\n");
	if (buf[256] != 0x55AA)
		print("valid bootloader magic\n");
	else
		print("invalid bootloader magic\n");
	struct highlow_64 sectors = drive_getsectors(bootdrive);
	printf("sectors low: %d\n", sectors.low);
	printf("sectors high: %d\n", sectors.high);
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
void outw(uint16_t port, uint16_t value){
	__asm__ volatile("outw %0, %1" : : "a"(value),"Nd"(port));
	return;
}
uint16_t inw(uint16_t port){
	uint16_t data = 0;
	__asm__ volatile("inw %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}
void outl(uint16_t port, uint32_t value){
	__asm__ volatile("outl %0, %1" : : "a"(value),"Nd"(port));
	return;
}
uint32_t inl(uint16_t port){
	uint32_t data = 0;
	__asm__ volatile("inl %1, %0" : "=a"(data) : "Nd"(port));
	return data;
}
