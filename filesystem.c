#include <stdint.h>
#include "bootloader.h"
#include "kernel.h"
#include "video.h"
#include "filesystem.h"
int read_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer){
        if (!buffer)
                return -1;
        unsigned int err = 0;
        while ((inb(0x1F7)&(1<<7))){};
        outb(0x1F2, sectorcnt&0xFF);
     	outb(0x0,0x0);
	outb(0x1F3, sector&0xFF);
	outb(0x0,0x0);
        outb(0x1F4, (sector>>8)&0xFF);
	outb(0x0,0x0);
        outb(0x1F5, (sector>>16)&0xFF);
	outb(0x0,0x0);
        outb(0x1F6, (drive|0xE0)|((sector>>24)&0x0F));
	outb(0x0,0x0);   
	outb(0x1F7, 0x20);
	outb(0x0,0x0);
        for (unsigned int i = 0;i<sectorcnt;i++){
                while ((inb(0x1F7)&(1<<7))){};
                if (inb(0x1F7)&0x1){
                        err = inb(0x1F1);
                        printf("failed to read sector %d (%p)\n", (void*)err);
                        return -1;
                }
                while (!(inb(0x1F7)&(1<<3))){};
                for (unsigned int s = 0;s<256;s++){
                        *buffer = inw(0x1F0);
                        buffer++;
                }
        } 
        return 0;
}
int write_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer){
	if (!buffer)
		return -1;
	unsigned int err = 0;
	while ((inb(0x1F7)&(1<<7))){};
	outb(0x1F2, sectorcnt&0xFF);
	outb(0x0,0x0);
	outb(0x1F3, sector&0xFF);
	outb(0x0,0x0);
	outb(0x1F4, (sector>>8)&0xFF);
	outb(0x0,0x0);
	outb(0x1F5, (sector>>16)&0xFF);
	outb(0x0,0x0);
	outb(0x1F6, (drive|0xE0)|((sector>>24)&0x0F));
	outb(0x0,0x0);
	outb(0x1F7, 0x30);
	outb(0x0,0x0);
	for (unsigned int i = 0;i<sectorcnt;i++){
		while ((inb(0x1F7)&(1<<7))){};
		for (unsigned int s = 0;s<256;s++){
			outw(0x1F0, *buffer);
			buffer++;
		}
		if (!(inb(0x1F7)&0x01))
			continue;
		err = inb(0x1F1);
		printf("failed to write sector %d to ata drive (0x%x)\n", i, err);
	}
	return 0;
}
int drive_getinfo(unsigned int drive, uint16_t* info){
	if (!info)
		return -1;
	unsigned int err = 0;
	while ((inb(0x1F7)&(1<<7))){};
	printf("ata controller is ready\n");
	outb(0x1F2, 0x0);
	outb(0x1F3, 0x0);
	outb(0x1F4, 0x0);
	outb(0x1F5, 0x0);
	outb(0x1F6, drive);
	outb(0x1F7, 0xEC);
	while ((inb(0x1F7)&(1<<7))){};
	for (unsigned int i = 0;i<256;i++){
		while ((inb(0x1F7)&(1<<7))){};
		if (inb(0x1F7)&0x1){
			err = inb(0x1F1);
			printf("failed to get drive info (0x%x)\n", err);
			return -1;
		}
		info[i] = inw(0x1F0);
	}
	return 0;
}
struct highlow_64 drive_getsectors(unsigned int drive){
	struct highlow_64 sectors = {0};
	uint16_t drive_info[256] = {0};
	if (drive_getinfo(drive, drive_info)!=0){
		print("failed to get drive info\n");
		return sectors;
	}
	sectors.low = (uint32_t)(drive_info[61]<<16)|(uint32_t)drive_info[60];
	sectors.high = ((uint32_t)drive_info[122]<<16)|(uint32_t)drive_info[123];
	return sectors;
}
unsigned int getbootdrive(void){
	struct bootloader_args* blargs = (struct bootloader_args*)0x2000;
	return blargs->bootdrive;
}
