#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "filesystem.h"
struct dirent* dirent = (struct dirent*)0x0;
size_t fsused = 4096;
unsigned char* fsbuf = (unsigned char*)0x0;
unsigned int* fat = (unsigned int*)0x0;
unsigned char* freelist = (unsigned char*)0x0;
unsigned char* data = (unsigned char*)0x0;
unsigned int fat_index = 0;
struct epic_fshdr* hdr = (struct epic_fshdr*)0x0;
unsigned int current_filecluster = 0;
unsigned int next_free_file = 0;
unsigned int allocate_cluster(void);
void free_cluster(unsigned int cluster);
int createfile(const char* filename, struct epic_file** pfiledata);
int writefile(unsigned int cluster, unsigned int clusteroff, const char* src);
unsigned int allocate_cluster(void){
	for (unsigned int i = 0;fat[i]!=EPICFS_EOC;i++){
		if (fat[i]!=EPICFS_FC)
			continue;
		fat[i]=i+1;
		return i;
	}
	unsigned int current_cluster = fat_index;
	fat[current_cluster]=current_cluster+1;
	freelist[current_cluster] = 0x1;
	fat_index++;
	return current_cluster;
}
void free_cluster(unsigned int cluster){
	fat[cluster] = EPICFS_FC;
	freelist[cluster] = 0x0;
	return;
}
int createfile(const char* filename, struct epic_file** pfiledata){
	if (!filename)
		return -1;
	if (!next_free_file||next_free_file>=4096/sizeof(struct epic_file)){
		printf("allocating new file data cluster\n");
		current_filecluster = allocate_cluster();
		struct epic_clusterhdr* pclusterhdr = (struct epic_clusterhdr*)(data+(4096*current_filecluster));
		pclusterhdr->type = CLUSTER_FILE;
		pclusterhdr->cluster = current_filecluster;
		hdr->last_filemd_cluster = current_filecluster;
	}
	unsigned int file_metadata_offset = (current_filecluster*4096)+sizeof(struct epic_clusterhdr)+(next_free_file*sizeof(struct epic_file));
	struct epic_file* pfile_metadata = (struct epic_file*)(data+file_metadata_offset);
	pfile_metadata->type = FILE_REGULAR;
	strcpy(pfile_metadata->filename, filename);
	pfile_metadata->size = 0;
	pfile_metadata->data = 0;
	pfile_metadata->last_data_cluster = 0;
	pfile_metadata->file_cluster = current_filecluster;
	pfile_metadata->file_offset = next_free_file;
	pfile_metadata->inuse = 1;
	*pfiledata = pfile_metadata;
	next_free_file++;
	return 0;
}
int writefile(unsigned int cluster, unsigned int clusteroff, const char* src){
	if (!src||fat[cluster]==EPICFS_EOC||fat[cluster]==EPICFS_FC)
		return -1;
	FILE* srcfile = fopen(src, "rb");
	unsigned char* srcbuf = (unsigned char*)0x0;
	unsigned int srcsize = 0;
	unsigned int file_sectors = 0;
	if (!srcfile){
		printf("failed to open source file %s (%s)\n", src, strerror(errno));
		return -1;
	}
	fseek(srcfile,0,SEEK_END);
	srcsize = (unsigned int)ftell(srcfile);
	file_sectors = 1+((srcsize-1)/512);
	rewind(srcfile);
	srcbuf = (unsigned char*)malloc(srcsize+4096-(srcsize%4096));
	if (!srcbuf){
		printf("failed to allocate memory for source file (%s)\n", strerror(errno));
		fclose(srcfile);
		return -1;
	}
	unsigned int bytes_read = (unsigned int)fread((void*)srcbuf, 1, srcsize, srcfile);	
	fclose(srcfile);
	struct epic_file* pepic_file = (struct epic_file*)(data+(cluster*4096)+(clusteroff*sizeof(struct epic_file))+sizeof(struct epic_clusterhdr));
	struct epic_clusterhdr* pclusterhdr = (struct epic_clusterhdr*)(data+(cluster*4096));
	if (pclusterhdr->type!=CLUSTER_FILE){
		printf("invalid file!\n");
		free(srcbuf);
		return -1;
	}
	pepic_file->size = srcsize;
	unsigned int last_cluster = 0;
	for (unsigned int i = 0;i<file_sectors;i++){
		unsigned int new_cluster = allocate_cluster();
		if (!i)
			pepic_file->data = new_cluster;
		else{
			fat[last_cluster] = new_cluster;
		}
		unsigned char* src_chunk = srcbuf+(4096*i);
		memcpy((void*)(data+(4096*new_cluster)), (const void*)src_chunk, 4096);
		pepic_file->last_data_cluster = new_cluster;
		last_cluster = new_cluster;
	}
	free(srcbuf);
	return 0;
}
int main(int argc, char** argv){
	if (argc<3)
		return -1;
	char* maindir = argv[1];
	char* outputfs = argv[2];
	char* strdrivesize = argv[3];
	if (!maindir||!outputfs||!strdrivesize)
		return -1;
	size_t drivesize = strtoull(strdrivesize, NULL, 0);
	size_t sizedt = drivesize%512;
	if (sizedt)
		drivesize+=(512-sizedt);
	DIR* dir = opendir(maindir);
	if (!dir){
		printf("failed to open dir (%s)\n", strerror(errno));
		return -1;
	}
	dirent = (struct dirent*)0x0;
	fsused = 4096;
	fsbuf = (unsigned char*)malloc(drivesize);
	if (!fsbuf){
		printf("failed to allocate memory for filesystem (%s)\n", strerror(errno));
		return -1;
	}
	fat = (unsigned int*)(fsbuf+4096);
	data = (unsigned char*)0x0;
	fat_index = 0;
	hdr = (struct epic_fshdr*)fsbuf;
	hdr->signature = EPICFS_SIGNATURE;
	hdr->bytes_per_cluster = 4096;
	hdr->fat_size = (drivesize/4096)*4;
	hdr->fat_size+=512-(hdr->fat_size%512);
	hdr->freelist_off = 129+(hdr->fat_size/512);
	hdr->freelist_size = drivesize/4096;
	hdr->freelist_size+=512-(hdr->freelist_size%512);
	hdr->data_off = FS_RESERVED_SECTORS+1+(hdr->fat_size/512)+(hdr->freelist_size/512);
	data = (unsigned char*)(fsbuf+512+hdr->fat_size+hdr->freelist_size);
	freelist = (unsigned char*)(fsbuf+512+hdr->fat_size);
	memset((void*)(fsbuf+512), 0, hdr->fat_size);
	memset((void*)(freelist), 0, hdr->freelist_size);
	while ((dirent=readdir(dir))){
		if (dirent->d_type!=DT_REG)
			continue;
		char fullpath[256] = {0};
		snprintf(fullpath, sizeof(fullpath), "%s/%s", maindir, dirent->d_name);
		printf("file name: %s\n", dirent->d_name);
		struct epic_file* pfile_md = (struct epic_file*)0x0;
		if (createfile(dirent->d_name, &pfile_md)!=0){
			printf("failed to create file (%s)\n", dirent->d_name);
			continue;
		}
		if (writefile(pfile_md->file_cluster, pfile_md->file_offset, fullpath)!=0){
			printf("failed to write file (%s)\n", dirent->d_name);
			continue;
		}
	}
	closedir(dir);
	fat[hdr->fat_size/512] = EPICFS_EOC;
//	fsused = (fat_index*4)+4096;
	fsused = drivesize;
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
