#include "video.h"
#include "bootloader.h"
#include "interrupt.h"
#include "stdlib.h"
#include "keyboard.h"
#include "commands.h"
#include "cursor.h"
#include "panic.h"
#include "timer.h"
#include "memory.h"
#include "thread.h"
#include "filesystem.h"
#include "kernel.h"
unsigned char shiftPressed = 0;
unsigned char capsPressed = 0;
char cmdline[256] = {0};
size_t cmdlen = 0;
void test_thread(void* arg);
void test_thread2(void* arg);
void kentry(void){
	struct bootloader_args* blargs = (struct bootloader_args*)0x2000;
	unsigned int avalibleMemory = 0;
	uint64_t installedMemory = 0;
	unsigned int bootdrive = getbootdrive();
	if (idt_init()!=0){
		panic("failed to load idt\n");
		__asm__ volatile("hlt");
		return;
	}
	cursor_enable(0x00, 15);
	if (heap_init()!=0){
		panic("failed to initialize kernel heap\n");
		return;
	}
	printf("cpu vendor: %s\n", blargs->vendorid);
	printf("cpu product name: %s\n", blargs->productname);
	for (unsigned int i = 0;i<blargs->memorymap_entries;i++){
		struct memorymap_entry entry = blargs->memorymap[i];
		if (entry.type!=5)
			continue;
		printf("corrupted memory at %p with size %d. Consider replacing your ram\n", (void*)entry.baselow, (int)entry.sizelow);
	}
	avalibleMemory = getAvalibleMemory();
	installedMemory = getInstalledMemory();
	if (avalibleMemory<2000000){
		panic("this operating system needs atleast 2mb of memory to run!\n");
		return;
	}
	printf("%d KB of memory avalible\n", ((int)avalibleMemory/1000));
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
	struct highlow_64 sectors = drive_getsectors(bootdrive);
	uint16_t first_sector[256] = {0};
	struct FAT32_FS_INFO* fsinfo = (struct FAT32_FS_INFO*)first_sector;
	unsigned int first_data_sector = 0;
	unsigned int firstSectorOfCluster = 0;
	if (read_sectors(bootdrive, FS_RESERVED_SECTORS, 1, first_sector, 256)!=0){
		panic("failed to read file system info from boot drive\n");
		return;
	}
	if (memcmp((void*)fsinfo->fileSystemType, (void*)"FAT32   ", 8)!=0){
		panic("not booted from valid fat32 drive\n");
		return;
	}
	if (!fsinfo->totalSectors32){
		fsinfo->totalSectors32 = sectors.low;
		print("total sectors not set! Setting total sectors\n");
		if (!write_sectors(bootdrive, FS_RESERVED_SECTORS, 1, first_sector, 256))
			print("successfully written with total sectors set\n");	
		else
			print("failed to write with total sectors\n");
	}
	first_data_sector = fsinfo->reservedSectorCount+(fsinfo->numFATs*fsinfo->FATSize32);
	uint32_t totalSectors = fsinfo->totalSectors32;
	uint32_t fatSize = fsinfo->FATSize32;
	uint32_t dataSectors = totalSectors-(fsinfo->reservedSectorCount+(fsinfo->numFATs*fatSize));
	uint32_t clustercnt = (dataSectors)/fsinfo->sectorsPerCluster;
	uint32_t bytes_per_cluster = fsinfo->sectorsPerCluster*fsinfo->bytesPerSector;
	printf("data sectors: %d\n", dataSectors);
	printf("fat size: %d\n", fatSize);
	printf("total sectors: %d\n", totalSectors);
	printf("cluster count: %d\n", clustercnt);	
	struct thread_t* thread = thread_create((uint32_t)test_thread, 0x1000);
	if (!thread)
		panic("failed to create test thread\n");
	struct thread_t* thread2 = thread_create((uint32_t)test_thread2, 0x1000);
	if (!thread2)
		panic("failed to create test thread 2\n");
	printf("thread %p: eip: %p, esp: %p, tid: %d, blink: %p, flink: %p\n", (void*)thread, thread->state.eip, thread->state.esp, thread->id, thread->blink, thread->flink);
	set_multithreading(1);
	while (1){};
	return;	
}
void test_thread(void* arg){
	printf("test thread started\n");
	while (1){
		sleep(1000);
		print("test thread running in loop...\n");
	}
	return;
}
void test_thread2(void* arg){
	printf("test thread 2 started\n");
	while (1){
		sleep(500);
		printf("test thread 2 running in loop\n");
	}
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
