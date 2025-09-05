#include <stdint.h>
#include "bootloader.h"
#include "kernel.h"
#include "video.h"
#include "stdlib.h"
#include "panic.h"
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
	static unsigned int last_drive = 0;
	static struct epic_fshdr last_info = {0};
	if (drive==last_drive){
		*pinfo = last_info;
		return 0;
	}
	unsigned char data[4096] = {0};
	if (read_sectors(drive, 128, 8, (uint16_t*)data, 256)!=0)
		return -1;
	last_info = *(struct epic_fshdr*)data;
	last_drive = drive;
	*pinfo = *(struct epic_fshdr*)data;
	return 0;
}
int epic_alloc_cluster(unsigned int drive, unsigned int* pcluster){
	if (!pcluster)
		return -1;
	unsigned int current_cluster = 0;
	while (1){
		unsigned int next_cluster = 0;
		if (epic_readcluster(drive, current_cluster, &next_cluster)!=0)
			return -1;
		if (next_cluster==EPICFS_EOC)
			return -1;
		if (next_cluster!=EPICFS_FC)
			continue;
		*pcluster = current_cluster;
		return epic_writecluster(drive, current_cluster, current_cluster+1);
	}
	return -1;
}
int epic_freecluster(unsigned int drive, unsigned int cluster){
	unsigned int cluster_sector = FS_RESERVED_SECTORS+1+((cluster*4)/512);
	unsigned int cluster_data[128] = {0};
	unsigned int cluster_index = cluster%512;
	if (read_sectors(drive, cluster_sector, 1, (uint16_t*)cluster_data, 256)!=0){
		return -1;
	}
	cluster_data[cluster_index] = EPICFS_FC;
	return write_sectors(drive, cluster_sector, 1, (uint16_t*)cluster_data, 256);
}
int epic_writecluster(unsigned int drive, unsigned int cluster, unsigned int data){
	unsigned int cluster_sector = FS_RESERVED_SECTORS+1+((cluster*4)/512);
	unsigned int cluster_data[128] = {0};
	unsigned int cluster_index = cluster%512;
	if (read_sectors(drive, cluster_sector, 1, (uint16_t*)cluster_data, 256)!=0)
		return -1;
	cluster_data[cluster_index] = data;
	return write_sectors(drive, cluster_sector, 8, (uint16_t*)cluster_data, 256);
}
int epic_readcluster(unsigned int drive, unsigned int cluster, unsigned int* pdata){
	unsigned int cluster_sector = FS_RESERVED_SECTORS+1+((cluster*4)/512);
	unsigned int cluster_data[128] = {0};
	unsigned int cluster_index = cluster%512;
	if (read_sectors(drive, cluster_sector, 1, (uint16_t*)cluster_data, 256)!=0)
		return -1;
	*pdata = cluster_data[cluster_index];
	return 0;
}
int epic_write_clusterdata(unsigned int drive, unsigned int cluster, unsigned char* pdata){
	if (!pdata)
		return -1;
	struct epic_fshdr fshdr = {0};
	if (epic_get_fsinfo(drive, &fshdr)!=0)
		return -1;
	if (fshdr.signature!=EPICFS_SIGNATURE)
		return -1;
	unsigned int clusterdata_sector = fshdr.data_off+(cluster*8);
	return write_sectors(drive, clusterdata_sector, 8, (uint16_t*)pdata, 256);
}
int epic_read_clusterdata(unsigned int drive, unsigned int cluster, unsigned char* pdata){
	if (!pdata)
		return -1;
	struct epic_fshdr fshdr = {0};
	if (epic_get_fsinfo(drive, &fshdr)!=0)
		return -1;
	if (fshdr.signature!=EPICFS_SIGNATURE)
		return -1;
	unsigned int clusterdata_sector = fshdr.data_off+(cluster*8);
	return read_sectors(drive, clusterdata_sector, 8, (uint16_t*)pdata, 256);
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
	unsigned char cluster_data[4096] = {0};
	unsigned int max_files = (sizeof(cluster_data)-sizeof(struct epic_clusterhdr))/sizeof(struct epic_file);
	while (1){
		printf("cluster: %d | ", current_cluster);
		unsigned int next_cluster = 0;
		if (epic_readcluster(drive, current_cluster,&next_cluster)!=0)
			return (struct file*)0x0;
		if (next_cluster==EPICFS_EOC)
			return (struct file*)0x0;
		if (next_cluster==EPICFS_FC)
			next_cluster = current_cluster+1;
		if (epic_read_clusterdata(drive, current_cluster, cluster_data)!=0)
			return (struct file*)0x0;
		struct epic_clusterhdr* clusterhdr = (struct epic_clusterhdr*)cluster_data;
		if (clusterhdr->type != CLUSTER_FILE){
			current_cluster = next_cluster;
			continue;
		}
		unsigned int current_file_index = 0;
		struct epic_file* filelist = (struct epic_file*)((unsigned char*)cluster_data+sizeof(struct epic_clusterhdr));
		for (unsigned int i = 0;i<max_files;i++){
			struct epic_file* current_file = filelist+i;
			if (current_file->type==FILE_INVALID)
				break;
			if (strcmp(current_file->filename, (char*)filename)!=0){
			current_file_index++;
			continue;
			}	
			struct file* newfile = (struct file*)kmalloc(sizeof(struct file)+sizeof(struct epic_fshdr));
			if (!newfile)
				return (struct file*)0x0;
			*(struct epic_fshdr*)(newfile+1) = fshdr;
			newfile->fstype = FS_EPIC;
			newfile->filetype = FILE_REGULAR;
			newfile->drive = drive;
			newfile->file_cluster = current_cluster;
			newfile->file_offset = i*sizeof(struct epic_file);
			return newfile;
		}
		current_cluster = next_cluster;
	}
	return (struct file*)0x0;
}
int renamefile(struct file* pfile, const char* newname){
	if (!pfile)
		return -1;
	if (pfile->fstype!=FS_EPIC)
		return -1;
	struct epic_fshdr* pfshdr = (struct epic_fshdr*)(pfile+1);
	if (pfshdr->signature!=EPICFS_SIGNATURE)
		return -1;
	unsigned char sector_data[4096] = {0};
	struct epic_file* pfile_data = (struct epic_file*)(sector_data+pfile->file_offset+sizeof(struct epic_clusterhdr));
	unsigned int filedata_sector = pfshdr->data_off+(pfile->file_cluster*8);
	if (read_sectors(pfile->drive, filedata_sector, 8, (uint16_t*)sector_data, 256)!=0){
		return -1;
	}
	strcpy(pfile_data->filename, newname);
	return write_sectors(pfile->drive, filedata_sector, 8, (uint16_t*)sector_data, 256);
}
int createfile(unsigned int drive, const char* filename){
	if (!filename)
		return -1;
	unsigned char fsinfo_sector_data[512] = {0};
	struct epic_fshdr* pfshdr = (struct epic_fshdr*)fsinfo_sector_data;
	if (epic_get_fsinfo(drive,(struct epic_fshdr*)fsinfo_sector_data)!=0){
		return -1;
	}
	if (pfshdr->signature!=EPICFS_SIGNATURE)
		return -1;
	unsigned int filemd_sector = pfshdr->data_off+(pfshdr->last_filemd_cluster*8);
	unsigned char sector_data[4096] = {0};
	if (read_sectors(drive, filemd_sector, 8, (uint16_t*)sector_data, 256)!=0)
		return -1;
	struct epic_file* fentries = (struct epic_file*)(sector_data+sizeof(struct epic_clusterhdr));
	for (unsigned int i = 0;i<sizeof(sector_data)/sizeof(struct epic_file);i++){
		struct epic_file* pfile = fentries+i;
		if (pfile->inuse!=0)
			continue;
		strcpy(pfile->filename, filename);
		pfile->inuse = 1;
		pfile->type = FILE_REGULAR;
		return write_sectors(drive, filemd_sector, 8, (uint16_t*)sector_data, 256);
	}
	unsigned int current_cluster = 0;
	unsigned int last_cluster = 0;
	unsigned int new_cluster = 0;
	if (epic_alloc_cluster(drive,&new_cluster)!=0)
		return -1;
	unsigned char newcluster_data[4096] = {0};
	struct epic_file* newfile = (struct epic_file*)newcluster_data;
	unsigned int clusterdata_sector = pfshdr->data_off+(new_cluster*8);
	if (read_sectors(drive, clusterdata_sector, 8, (uint16_t*)newcluster_data, 256)!=0)
		return -1;
	strcpy(newfile->filename, filename);
	newfile->inuse = 1;
	newfile->type = FILE_REGULAR;
	return write_sectors(drive, clusterdata_sector,8, (uint16_t*)newcluster_data, 256);
}
int deletefile(struct file* pfile){
	if (!pfile)
		return -1;
	if (pfile->fstype!=FS_EPIC)
		return -1;
	struct epic_fshdr* pfshdr = (struct epic_fshdr*)(pfile+1);
	if (pfshdr->signature!=EPICFS_SIGNATURE)
		return -1;
	unsigned int file_md_cluster = pfile->file_cluster;
	unsigned int file_md_sector = pfshdr->data_off+(file_md_cluster*8);
	unsigned char file_data[4096] = {0};
	if (read_sectors(pfile->drive, file_md_sector, 8, (uint16_t*)file_data, 256)!=0)
		return -1;
	struct epic_file* pepicfile = (struct epic_file*)(file_data+sizeof(struct epic_clusterhdr)+pfile->file_offset);
	unsigned int current_cluster = pepicfile->data;
	unsigned int current_sector = 0;
	unsigned int last_sector = 0;
	unsigned int current_data_sector = 0;
	unsigned int filedata_clusters = 1+((pepicfile->size-1)/4096);
	pepicfile->inuse = 0;
	if (write_sectors(pfile->drive, file_md_sector, 8, (uint16_t*)file_data, 256)!=0)
		return -1;
	if (!pepicfile->size)
		return 0;
	for (unsigned int i = 0;i<filedata_clusters;i++){
		unsigned int next_cluster = 0;
		if (epic_readcluster(pfile->drive, current_cluster, &next_cluster)!=0)
			return -1;
		if (next_cluster==EPICFS_EOC)
			return -1;
		if (next_cluster==EPICFS_FC){
			current_cluster++;
			continue;
		}
		if (epic_freecluster(pfile->drive, current_cluster)!=0)
			return -1;
		current_cluster = next_cluster;
	}
	return 0;
}
int readfile(struct file* pfile, unsigned char* buffer){
	if (!pfile||!buffer)
		return -1;
	if (pfile->fstype!=FS_EPIC)
		return -1;
	struct epic_fshdr* pfshdr = (struct epic_fshdr*)(pfile+1);
	if (pfshdr->signature!=EPICFS_SIGNATURE)
		return -1;
	unsigned int file_md_sector = pfshdr->data_off+(pfile->file_cluster*8);
	unsigned char sector_data[4096] = {0};
	if (epic_read_clusterdata(pfile->drive, pfile->file_cluster, (unsigned char*)sector_data)!=0)
		return -1;
	struct epic_file* pfdata = (struct epic_file*)(sector_data+sizeof(struct epic_clusterhdr)+pfile->file_offset);
	unsigned int data_clustercnt = 1+((pfdata->size-1)/4096);
	unsigned int current_cluster = pfdata->data;
	unsigned int last_cluster = 0;
	if (!pfdata->size)
		return 0;
	for (unsigned int i = 0;i<data_clustercnt;i++){
		unsigned int next_cluster = 0;
		if (epic_readcluster(pfile->drive, current_cluster, &next_cluster)!=0){
			panic("failed to read next cluster\n");
			return -1;	
		}
		if (read_sectors(pfile->drive, pfshdr->data_off+(current_cluster*8), 8, (uint16_t*)(buffer+(4096*i)), 256)!=0){
			return -1;
		}
		current_cluster = next_cluster;
	}
	return 0;
}
int writefile(struct file* pfile, unsigned char* buffer, unsigned int size){
	if (!pfile||!buffer||!size)
		return -1;
	if (pfile->fstype!=FS_EPIC)
		return -1;
	struct epic_fshdr* pfshdr = (struct epic_fshdr*)(pfile+1);
	if (pfshdr->signature!=EPICFS_SIGNATURE)
		return -1;
	unsigned char filemd_data[4096] = {0};
	unsigned int md_offset = pfile->file_offset+sizeof(struct epic_clusterhdr);
	unsigned int md_sector = pfshdr->data_off+(pfile->file_cluster*8);
	struct epic_file* pfilemd = (struct epic_file*)(filemd_data+md_offset);
	if (read_sectors(pfile->drive, md_sector, 1, (uint16_t*)filemd_data, 256)!=0)
		return -1;
	unsigned int file_clusters = 1+((pfilemd->size-1)/4096);
	unsigned int clusters_needed = 1+((size-1)/4096);
	unsigned int current_cluster = pfilemd->data;
	unsigned int current_cluster_sector = 0;
	unsigned int last_cluster = 0;
	unsigned int last_cluster_sector = 0;
	unsigned int cluster_data[128] = {0};
	unsigned int clusters_tofree = 0;
	if (file_clusters>clusters_needed)
		clusters_tofree = file_clusters-clusters_needed;
	current_cluster = pfilemd->data;
	unsigned int clusters_toalloc = 0;
	if (file_clusters<clusters_needed)
		clusters_toalloc = clusters_needed-file_clusters;
	for (unsigned int i = 0;i<clusters_toalloc;i++){
		unsigned int new_cluster = 0;
		if (epic_alloc_cluster(pfile->drive, &new_cluster)!=0)
			return -1;
		if (pfilemd->last_data_cluster){
			if (epic_writecluster(pfile->drive, pfilemd->last_data_cluster, new_cluster)!=0)
				return -1;
		}
		pfilemd->last_data_cluster = new_cluster;
	}
	current_cluster = pfilemd->data;
	unsigned int clusters_found = 0;
	for (unsigned int i = 0;i<clusters_tofree;clusters_found++){
		unsigned int next_cluster = 0;
		if (epic_readcluster(pfile->drive, current_cluster, &next_cluster)!=0)
			return -1;
		if (clusters_found<file_clusters-clusters_tofree){
			clusters_found++;
			current_cluster = next_cluster;
			continue;
		}
		if (epic_freecluster(pfile->drive, current_cluster)!=0)
			return -1;
		clusters_found++;
		i++;
		current_cluster = next_cluster;
	}
	current_cluster = pfilemd->data;
	for (unsigned int i = 0;i<clusters_needed;i++){
		unsigned int next_cluster = 0;
		if (epic_readcluster(pfile->drive, current_cluster, &next_cluster)!=0){
			return -1;
		}
		unsigned int data_towrite = 4096;
		unsigned int dt = size%4096;
		if (i==clusters_needed&&dt)
			data_towrite = dt;
		unsigned int cluster_sector = pfshdr->data_off+(current_cluster*8);
		unsigned int bufindex = 4096*i;
		if (write_sectors(pfile->drive, cluster_sector, 1, (uint16_t*)(buffer+bufindex), 256)!=0)
			return -1;
		current_cluster = next_cluster;
	}
	return epic_write_clusterdata(pfile->drive, pfile->file_cluster, (unsigned char*)filemd_data);
}
unsigned int getfilesize(struct file* pfile){
	if (!pfile){
		printf("invalid file\n");
		return 0;
	}
	if (pfile->fstype!=FS_EPIC){
		printf("invalid file system type\n");
		return 0;
	}
	struct epic_fshdr* pfshdr = (struct epic_fshdr*)(pfile+1);
	if (pfshdr->signature!=EPICFS_SIGNATURE)
		return -1;
	unsigned char sector_data[4096] = {0};
	unsigned int cluster_sector = (pfshdr->data_off+(pfile->file_cluster*8));
	if (read_sectors(pfile->drive, cluster_sector, 8, (uint16_t*)sector_data, 256)!=0){
		printf("failed to get file info\n");
		return 0;
	}
	struct epic_file* pfile_md = (struct epic_file*)(sector_data+pfile->file_offset+sizeof(struct epic_clusterhdr));
	return pfile_md->size;
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
