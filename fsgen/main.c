#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "../filesystem.h"
#include <stdio.h>
int main(int argc, char** argv){
	struct dirent* dirent = {0};
	DIR* dir = opendir("fs");
	if (!dir){
		printf("failed to open file system dir\n");
		return -1;
	}
	size_t filesystemMax = sizeof(struct fs_link);
	size_t filesystemused = 0x0;
	unsigned char* filesystem = (unsigned char*)malloc(filesystemMax);
	if (!filesystem){
		printf("failed to allocate memory for file system\n");
		closedir(dir);
		return -1;
	}
	unsigned int currentoff = 0;
	while ((dirent=readdir(dir))){
		switch(dirent->d_type){
			case DT_REG:{
				size_t newused = filesystemused+sizeof(struct fs_link);
				if (newused>=filesystemMax){
					unsigned char* oldfs = filesystem;
					filesystem = malloc(filesystemMax*2);
					if (!filesystem){
						printf("failed to reallocate file system (%s)\n", strerror(errno));
						free(filesystem);
						closedir(dir);
						return -1;
					}
					memcpy((void*)filesystem, (const void*)oldfs, filesystemMax);
					free(oldfs);
					filesystemMax*=2;
					printf("file system successfully reallocated\n");
				}
				printf("adding file %s to filesystem\n", dirent->d_name);
				struct fs_link* currentlink = (struct fs_link*)filesystem+currentoff;
				strcpy(currentlink->filename, dirent->d_name);
				currentlink->index = currentoff;
				currentoff++;
				filesystemused+=sizeof(struct fs_link);
				break;
			}
			case DT_DIR:

				break;
		}
	}
	FILE* output = fopen("filesystem.img", "wb");
	if (!output){
		printf("failed to open output image (%s)\n", strerror(errno));
		free(filesystem);
		closedir(dir);
		return -1;
	}
	size_t byteswritten = fwrite((const void*)filesystem, 1, filesystemused, output);
	if (byteswritten!=filesystemused)
		printf("only written %zu/%zu bytes of the file system\n", byteswritten, filesystemused);
	else
		printf("successfully written all %zu/%zu bytes of the file system\n", byteswritten, filesystemused);
	fclose(output);
	free(filesystem);
	closedir(dir);
	return 0;
}
