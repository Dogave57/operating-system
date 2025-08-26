#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include "../filesystem.h"
#define align_up(val, align)((val+align-1) & ~(align-1))
int main(int argc, char** argv){
	const char* filespath = "files";
	DIR* dir = (DIR*)NULL;
	struct dirent* dirent = (struct dirent*)NULL;
	unsigned char* fsbuf = (unsigned char*)NULL;
	size_t fs_size = 1024;
	size_t fs_used = sizeof(struct filesystem_info);
	struct filesystem_info* fs_info = (struct filesystem_info*)NULL;
	FILE* fsout = (FILE*)NULL;
	size_t byteswritten = 0;
	size_t fs_used_aligned = 0;
	dir = opendir(filespath);
	if (!dir){
		printf("failed to open files directory (%s)\n", strerror(errno));
		return -1;
	}
	fsbuf = (unsigned char*)malloc(fs_size);
	if (!fsbuf){
		printf("failed to allocate memory for file system (%s)\n", strerror(errno));
		closedir(dir);
		return -1;	
	}
	fs_info = (struct filesystem_info*)fsbuf;
	fs_info->signature = FS_SIGNATURE;
	strcpy(fs_info->fsname, "the best fs");
	while ((dirent=readdir(dir))){
		if (dirent->d_type!=DT_REG)
			continue;
		char path[256] = {0};
		FILE* file = (FILE*)NULL;
		size_t filesize = 0;
		size_t bytesread = 0;
		unsigned char* filecontents = (unsigned char*)NULL;
		snprintf(path, sizeof(path), "%s/%s", filespath, dirent->d_name);
		file = fopen(path, "rb");
		if (!file){
			printf("failed to open file at %s (%s)\n", path, strerror(errno));
			continue;
		}
		fseek(file,0,SEEK_END);
		filesize = ftell(file);
		rewind(file);
		filecontents = (unsigned char*)malloc(filesize);
		if (!filecontents){
			printf("failed to allocate memory for file contents for %s (%s)\n", path, strerror(errno));
			fclose(file);
			continue;
		}
		bytesread = fread((void*)filecontents, 1, filesize, file);
		if (bytesread!=filesize)
			printf("read %zu/%zu bytes of file at %s\n", bytesread, filesize, path);
		fclose(file);
		printf("adding file at %s to file system\n", path);
		free(filecontents);
	}	
	closedir(dir);
	fsout = (FILE*)fopen("filesystem.bin", "wb");
	if (!fsout){
		printf("failed to open filesystem output (%s)\n", strerror(errno));
		free(fsbuf);
		return -1;
	}
	fs_used_aligned = align_up(fs_used, 512);
	byteswritten = fwrite((void*)fsbuf, 1, fs_used_aligned, fsout);
	if (byteswritten!=fs_used_aligned){
		printf("only written %zu/%zu bytes of the file system\n", byteswritten, fs_used_aligned);
		fclose(fsout);
		free(fsbuf);
		return -1;
	}
	printf("file system used: %zu\n", byteswritten);
	fclose(fsout);
	free(fsbuf);
	return 0;
}
