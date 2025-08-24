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
#include "usb.h"
#include "smbios.h"
#include "pci.h"
#include "kernel.h"
unsigned char shiftPressed = 0;
unsigned char capsPressed = 0;
char cmdline[256] = {0};
size_t cmdlen = 0;
void test_thread(void* arg);
void test_thread2(void* arg);
void test_thread3(void* arg);
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
	if (usb_init()!=0){
		panic("failed to initialize usb\n");
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
	struct highlow_64 sectors = drive_getsectors(bootdrive);
	printf("drive sectors: %d\n", sectors.low);
	struct thread_t* thread = thread_create((uint32_t)test_thread, 0x1000, NULL);
	if (!thread)
		panic("failed to create test thread\n");
	struct thread_t* thread2 = thread_create((uint32_t)test_thread2, 0x1000, NULL);
	if (!thread2)
		panic("failed to create test thread 2\n");
	struct thread_t* thread3 = thread_create((uint32_t)test_thread3, 0x1000, NULL);
	for (unsigned int bus = 0;bus<256;bus++){
		for (unsigned int dev = 0;dev<32;dev++){
			for (unsigned int func = 0;func<8;func++){
				if (pci_device_exists(bus,dev,func)==0)
					continue;
				struct pci_bar_data bar_data = {0};
				uint32_t vendor_id = pci_get_vendor(bus,dev,func);
				uint32_t device_id = pci_get_devid(bus,dev,func);
				pci_get_bars(bus,dev,func,&bar_data);
				//printf("pci device at bus %d device %d func: %d vendor: %x device id: %x\n", bus, dev, func, vendor_id, device_id);
				for (unsigned int bar = 0;bar<6;bar++){
					struct pci_bar pcibar = bar_data.pcibars[bar];
					if (!pcibar.base)
						continue;
					if (pcibar.isX64)
						print("x64 bar\n");
//					else
//						print("x86 bar\n");
//					if (pcibar.barType==PCI_BAR_MMIO)
//						printf("mmio bar at %x\n", pcibar.base);
///					else
//						printf("io bar at: %x\n", pcibar.base);
				}
			}	
		}
	}
	uint32_t smbios = get_smbios();
	struct smbios_sysinfo* sysinfo = smbios_get_sysinfo(smbios);
	if (sysinfo==(struct smbios_sysinfo*)0x0){
		panic("failed to get smbios system info\n");
		__asm__ volatile("hlt");
	}
	char* strsect = ((char*)sysinfo)+sizeof(struct smbios_header)+sysinfo->header.len;
	unsigned int strindex = 0;
	for (unsigned int i = 0;;i++){
		if (strsect[i]!=0)
			continue;
		if (!strsect[i+1])
			break;
		strindex++;
		if (strindex==sysinfo->manufacturer_index){
		//	printf("manufacturer\n");
		}
	}
//	printf("version: %s\n", strsect+sysinfo->version_index);
//	printf("uuid: %x\n", sysinfo->uuid);
	set_multithreading(0);
	while (1){};
	return;	
}
void test_thread(void* arg){
	printf("test thread started\n");
	while (1){
		sleep(700);
		print("test thread 1 running in loop...\n");
	}
	return;
}
void test_thread2(void* arg){
	printf("test thread 2 started\n");
	while (1){
		sleep(700);
		printf("test thread 2 running in loop\n");
	}
	return;
}
void test_thread3(void* arg){
	printf("test thread 3 started\n");
	while (1){
		sleep(700);
		printf("test thread 3 running in loop\n");
	}
	return;
}
void keyboard_interrupt(void){
	if (!(inb(0x64)&1))
		return;
	uint8_t scancode = inb(0x60);
	switch (scancode){
		case 0x3A:
			capsPressed = 1;
			break;
		case 0xBA:
			capsPressed = 0;
			break;
		case 0x2A:
		case 0x36:
		//	__asm__ volatile("int $12");
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
//	__asm__ volatile("int $12");
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
