#include <stdint.h>
#include "bootloader.h"
#include "kernel.h"
#include "video.h"
#include "stdlib.h"
#include "filesystem.h"
int read_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int wordsPerSector){
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
                        printf("failed to read sector %d (%p)\n", sector+i,(void*)err);
                        return -1;
                }
                while (!(inb(0x1F7)&(1<<3))){};
                for (unsigned int s = 0;s<wordsPerSector;s++){
                        *buffer = inw(0x1F0);
                        buffer++;
                }
        } 
        return 0;
}
int write_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int wordsPerSector){
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
		for (unsigned int s = 0;s<wordsPerSector;s++){
			outw(0x1F0, *buffer);
			buffer++;
		}
		if (!(inb(0x1F7)&0x01))
			continue;
		err = inb(0x1F1);
		printf("failed to write sector %d to ata drive (0x%x)\n", sector+i, err);
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
int epic_get_fsinfo(unsigned int drive, struct epic_fshdr* pinfo){
	if (!pinfo)
		return -1;
	unsigned char data[512] = {0};
	if (read_sectors(drive, 128, 1, (uint16_t*)data, 256)!=0)
		return -1;
	*pinfo = *(struct epic_fshdr*)data;
	return 0;
}
struct file* openfile(unsigned int drive, const char* filename){
	if (!filename)
		return (struct file*)0x0;
	struct epic_fshdr fshdr = {0};
	if (epic_get_fsinfo(drive, &fshdr))
		return (struct file*)0x0;
	if (fshdr.signature!=EPICFS_SIGNATURE)
		return (struct file*)0x0;
	unsigned int current_cluster = 0;
	unsigned int current_sector = 0;
	unsigned int last_sector = 0;
	unsigned int cluster_metadata[128] = {0};
	while (1){
		current_sector = (FS_RESERVED_SECTORS+1)+((current_cluster*4)/512);
		if (last_sector!=current_sector)
			read_sectors(drive, current_sector, 1, (uint16_t*)cluster_metadata, 256);
		last_sector = current_sector;
		unsigned int sector_index = current_cluster%512;
		unsigned int next_cluster = cluster_metadata[sector_index];
		if (next_cluster==EPICFS_EOC){
			return (struct file*)0x0;
		}
		if (next_cluster==EPICFS_FC){
			current_cluster++;
			continue;
		}
		unsigned int cluster_data_sector = fshdr.data_off+((current_cluster));
		unsigned char cluster_data[512] = {0};
		if (read_sectors(drive, cluster_data_sector, 1, (uint16_t*)cluster_data, 256)!=0){
			printf("failed to read drive sectors for data sector\n");
			current_cluster = next_cluster;
			continue;	
		}
		struct epic_clusterhdr* clusterhdr = (struct epic_clusterhdr*)(cluster_data);
		if (clusterhdr->type!=CLUSTER_FILE){
			current_cluster = next_cluster;
			continue;
		}
		struct epic_file* pepic_file = (struct epic_file*)clusterhdr;
		if (strcmp(pepic_file->filename, (char*)filename)!=0){
			current_cluster = next_cluster;
			continue;
		}
		struct file* pfile = (struct file*)kmalloc(sizeof(struct file)+sizeof(struct epic_fshdr));
		if (!pfile)
			return pfile;
		*(struct epic_fshdr*)(pfile+1) = fshdr;
		pfile->fstype = FS_EPIC;
		pfile->filetype = FILE_REGULAR;
		pfile->len = sizeof(struct file)+sizeof(struct epic_fshdr);
		pfile->drive = drive;
		pfile->file_cluster = current_cluster;
		return pfile;
	}
	return (struct file*)0x0;
}
int renamefile(struct file* pfile, const char* newname){
	if (!pfile)
		return -1;
	if (pfile->fstype!=FS_EPIC)
		return -1;
	struct epic_fshdr* pfshdr = (struct epic_fshdr*)(pfile+1);
	unsigned char sector_data[512] = {0};
	struct epic_file* pfile_data = (struct epic_file*)(sector_data);
	unsigned int filedata_sector = pfshdr->data_off+pfile->file_cluster;
	if (read_sectors(pfile->drive, filedata_sector, 1, (uint16_t*)sector_data, 256)!=0){
		return -1;
	}
	strcpy(pfile_data->filename, newname);
	return write_sectors(pfile->drive, filedata_sector, 1, (uint16_t*)sector_data, 256);
}
int closefile(struct file* pfile){
	if (!pfile)
		return -1;
	kfree((void*)pfile);
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
