#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "filesystem.h"
int main(int argc, char** argv){
	if (argc<3)
		return -1;
	char* maindir = argv[1];
	char* outputfs = argv[2];
	char* strdrivesize = argv[3];
	if (!maindir||!outputfs||!strdrivesize)
		return -1;
	size_t drivesize = strtoull(strdrivesize, NULL, 0);
	if (drivesize%2)
		drivesize++;
	DIR* dir = opendir(maindir);
	if (!dir){
		printf("failed to open dir (%s)\n", strerror(errno));
		return -1;
	}
	struct dirent* dirent = (struct dirent*)0x0;
	size_t fsused = sizeof(struct fs_hdr);
	unsigned char* fsbuf = (unsigned char*)malloc(drivesize);
	if (!fsbuf){
		printf("failed to allocate memory for filesystem (%s)\n", strerror(errno));
		return -1;
	}
	unsigned int* fat = (unsigned int*)(fsbuf+sizeof(struct fs_hdr));
	unsigned int fat_index = 0;
	struct fs_hdr* hdr = (struct fs_hdr*)fsbuf;
	hdr->signature = FS_SIGNATURE;
	hdr->bytes_per_cluster = 512;
	hdr->fat_size = drivesize/32;
	hdr->data_off = hdr->fat_size/hdr->bytes_per_cluster;
	while ((dirent=readdir(dir))){
		if (dirent->d_type!=DT_REG)
			continue;
		char fullpath[256] = {0};
		snprintf(fullpath, sizeof(fullpath), "%s/%s", maindir, dirent->d_name);
		printf("file name: %s\n", dirent->d_name);
		FILE* newfile = fopen(fullpath, "rb");
		if (!newfile){
			printf("failed to open file at %s for reading (%s)\n", fullpath, strerror(errno));
			continue;
		}
		fseek(newfile,0,SEEK_END);
		uint64_t filesize = (uint64_t)ftell(newfile);
		rewind(newfile);
		fat[fat_index]=0x1;
		fatindex++;
		fclose(newfile);
	}
	closedir(dir);
	fat[fat_index] = EPICFS_EOC;
	size_t reserved_bytes = 512*128;
	unsigned char* outbuf = (unsigned char*)malloc(drivesize+reserved_bytes);
	if (!outbuf){
		printf("failed to allocate memory for output buffer (%s)\n", strerror(errno));
		free(fsbuf);
		return -1;
	}
	memcpy((void*)(outbuf+reserved_bytes), (const void*)fsbuf, fsused);
	free(fsbuf);
	FILE* outfile = fopen(outputfs, "rb");
	if (!outfile){
		printf("failed to open output file (%s)\n", strerror(errno));
		free(outbuf);
		return -1;
	}	
	fread((void*)outbuf,1,reserved_bytes, outfile);
	fclose(outfile);
	outfile = fopen(outputfs, "wb");
	if (!outfile){
		printf("failed to open output file (%s)\n", strerror(errno));
		free(outbuf);
		return -1;
	}
	fwrite((void*)outbuf, 1, drivesize+reserved_bytes, outfile);
	fclose(outfile);
	return 0;
}
