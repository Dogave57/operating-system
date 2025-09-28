#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "filesystem.h"
#define align_up(val, align)((val+align-1)& ~(align-1))
char* maindir = (char*)0x0;
size_t maindir_len = 0;
struct dirent* dirent = (struct dirent*)0x0;
size_t fsused = 4096;
unsigned char* fsbuf = (unsigned char*)0x0;
unsigned int* fat = (unsigned int*)0x0;
unsigned char* freelist = (unsigned char*)0x0;
unsigned char* data = (unsigned char*)0x0;
unsigned int fat_index = 0;
unsigned int fat_entries = 0;
struct epic_fshdr* hdr = (struct epic_fshdr*)0x0;
unsigned int current_filecluster = 0;
unsigned int next_free_file = 0;
unsigned int allocate_cluster(enum clusterType type);
void free_cluster(unsigned int cluster);
int findfile_incluster(unsigned int cluster, const char* filename, struct epic_file** ppfilemd);
int findfile_indir(struct epic_file* pfilemd, const char* filename, struct epic_file** ppfilemd);
int findfile_inroot(const char* filename, struct epic_file** pfilemd);
int createfile_incluster(unsigned int cluster, struct epic_file** pfiledata, const char* filename, enum fileType type);
int createfile_indir(struct epic_file* pdir, const char* filename, struct epic_file** ppfilemd, enum fileType type);
int createfile(const char* filename, struct epic_file** pfiledata, enum fileType type);
int writefile(unsigned int cluster, unsigned int clusteroff, const char* src);
int readfile(unsigned int cluster, unsigned int clusteroff, unsigned char* buf);
int adddir(const char* dirname);
unsigned int allocate_cluster(enum clusterType type){
	for (unsigned int i = 1;i<fat_entries;i++){
		if (freelist[i]!=0x0)
			continue;
		fat[i]=i+1;
		freelist[i] = 0x1;
		printf("cluster %d entry index %d\n", i, i);
		unsigned char* pclusterdata = (unsigned char*)data+(i*4096);
		struct epic_clusterhdr* pclusterhdr = (struct epic_clusterhdr*)pclusterdata;
		pclusterhdr->type = type;
		pclusterhdr->cluster = i;
		if (i>hdr->last_cluster)
			hdr->last_cluster = i;
		return i;
	}
	printf("no entries\n");
	return 0;
}
void free_cluster(unsigned int cluster){
	fat[cluster] = EPICFS_FC;
	freelist[cluster] = 0x0;
	return;
}
int findfile_incluster(unsigned int cluster, const char* filename, struct epic_file** ppfilemd){
	if (!filename||!ppfilemd)
		return -1;
	unsigned char* clusterdata = (unsigned char*)(data+(cluster*4096));
	struct epic_clusterhdr* clusterhdr = (struct epic_clusterhdr*)(clusterdata);
	struct epic_file* pfilelist = (struct epic_file*)(clusterdata+sizeof(struct epic_clusterhdr));
	unsigned int max_files = (4096-sizeof(struct epic_clusterhdr))/sizeof(struct epic_file);
	for (unsigned int i = 0;i<max_files;i++){
		struct epic_file* pfile_entry = (struct epic_file*)(pfilelist+i);
		if (!pfile_entry->inuse)
			continue;
		if (strcmp(pfile_entry->filename, filename)!=0)
			continue;
		*ppfilemd = pfile_entry;
		return 0;
	}
	return -1;
}
int findfile_indir(struct epic_file* pfilemd, const char* filename, struct epic_file** ppfilemd){
	if (!filename||!ppfilemd||!pfilemd)
		return -1;
	unsigned int current_cluster = pfilemd->data;
	unsigned int file_clusters = 0;
	if (pfilemd->size!=0)
		file_clusters = 1+((file_clusters-1)/4096);
	for (unsigned int i = 0;i<file_clusters;i++){
		if (fat[current_cluster]==EPICFS_EOC)
			return -1;
		if (findfile_incluster(current_cluster, filename, ppfilemd)!=0){
		current_cluster = fat[current_cluster];
		continue;
		}
		return 0;
	}
	return -1;
}
int findfile_inroot(const char* filename, struct epic_file** ppfilemd){
	if (!filename||!ppfilemd)
		return -1;
	unsigned int current_cluster = 0;
	unsigned int max_files = (4096-sizeof(struct epic_clusterhdr))/sizeof(struct epic_file);
	for (current_cluster=0;fat[current_cluster]!=EPICFS_EOC;current_cluster++){
		if (findfile_incluster(current_cluster, filename, ppfilemd)!=0)
			continue;
		return 0;	
	}
	return -1;
}
int createfile_incluster(unsigned int cluster, struct epic_file** pfiledata, const char* filename, enum fileType type){
	if (!pfiledata||!filename)
		return -1;
	unsigned int file_entries = (4096-sizeof(struct epic_clusterhdr))/sizeof(struct epic_file);
	unsigned char* pclusterdata = data+(cluster*4096);
	struct epic_clusterhdr* pclusterhdr = (struct epic_clusterhdr*)pclusterdata;
	struct epic_file* pfilelist = (struct epic_file*)(pclusterhdr+1);
	if (pclusterhdr->type==CLUSTER_INVALID)
		return -1;
	for (unsigned int i = 0;i<file_entries;i++){
		if (pfilelist[i].inuse!=0)
			continue;
		strcpy(pfilelist[i].filename, filename);
		pfilelist[i].inuse = 1;
		pfilelist[i].type = type;
		pfilelist[i].file_cluster = cluster;
		pfilelist[i].file_offset = i*sizeof(struct epic_file);
		*pfiledata = pfilelist+i;
		printf("created %s in cluster %d\n", filename, cluster);
		return 0;
	}
	return -1;	
}
int createfile_indir(struct epic_file* pdir, const char* filename, struct epic_file** pfiledata, enum fileType type){
	if (!pdir||!filename||!pfiledata)
		return -1;
	if (pdir->type!=FILE_DIR)
		return -1;
	unsigned int file_clusters = 0;
	unsigned int current_cluster = pdir->data;	
	if (pdir->size!=0)
		file_clusters = 1+((pdir->size-1)/4096);
	for (unsigned int i = 0;i<file_clusters&&pdir->size!=0&&pdir->data!=0;i++){
		if (current_cluster==EPICFS_EOC)
			break;
		if (current_cluster==EPICFS_FC)
			break;
		pdir->size+=sizeof(struct epic_file);
		if (createfile_incluster(current_cluster, pfiledata, filename, type)!=0){
			current_cluster = fat[current_cluster];
			continue;
		}
		pdir->parent_cluster = pdir->file_cluster;
		pdir->parent_offset = pdir->file_offset;
		return 0;
	}
	unsigned int new_cluster = allocate_cluster(CLUSTER_FILEDATA);
	if (!new_cluster)
		return -1;
	if (!pdir->data)
		pdir->data = new_cluster;
	if (pdir->last_data_cluster!=0)
		fat[pdir->last_data_cluster] = new_cluster;
	pdir->last_data_cluster = new_cluster;
	pdir->parent_cluster = pdir->file_cluster;
	pdir->parent_offset = pdir->file_offset;
	pdir->size+=sizeof(struct epic_file);
	printf("created %s in new dir cluster %d\n", filename, pdir->file_cluster);
	return createfile_incluster(new_cluster, pfiledata, filename, type);
}
int createfile(const char* filename, struct epic_file** pfiledata, enum fileType type){
	if (!filename||!pfiledata)
		return -1;
	char filename_cpy[256] = {0};
	unsigned int tocpy = strlen(filename);
	if (tocpy>sizeof(filename_cpy)-1)
		tocpy = sizeof(filename_cpy)-1;
	memcpy((void*)filename_cpy, (const void*)filename, tocpy);
	unsigned int link_start = 0;
	struct epic_file* current_dir = (struct epic_file*)0x0;
	unsigned int current_cluster = 0;
	for (unsigned int i = 0;i<256;i++){
		if (!filename_cpy[i]){
			if (!current_dir){
			break;
			}
			if (createfile_indir(current_dir, filename_cpy+link_start, pfiledata, type)!=0){
			printf("failed to create %s in %s\n", filename_cpy+link_start, current_dir->filename);
			return -1;
			}
			printf("successfully created %s in %s\n", filename_cpy+link_start, current_dir->filename);
			return 0;	
		}
		if (filename_cpy[i]!='/')
			continue;
		filename_cpy[i] = 0;
		if (current_dir==(struct epic_file*)0x0){
			if (findfile_inroot(filename_cpy+link_start, &current_dir)!=0){
			printf("failed to find %s in root\n",filename);
			return -1;
			}
			link_start = i+1;
			continue;
		}
		if (findfile_indir(current_dir, filename_cpy+link_start, &current_dir)!=0){
			printf("failed to find %s in dir %s\n", filename_cpy+link_start, current_dir->filename);
			return -1;
		}
		link_start = i+1;
	}
	unsigned int max_file_entries = (4096-sizeof(struct epic_clusterhdr))/sizeof(struct epic_file);
	if (!next_free_file||next_free_file>max_file_entries||!current_filecluster){
		current_filecluster = allocate_cluster(CLUSTER_FILEDATA);
		if (!current_filecluster){
			printf("failed to allocate new file data cluster\n");
			return -1;	
		}
		struct epic_clusterhdr* pclusterhdr = (struct epic_clusterhdr*)(data+(4096*current_filecluster));
		pclusterhdr->type = CLUSTER_FILE;
		pclusterhdr->cluster = current_filecluster;
		hdr->last_filemd_cluster = current_filecluster;
		next_free_file = 0;
	}
	unsigned int file_metadata_offset = (current_filecluster*4096)+sizeof(struct epic_clusterhdr)+(next_free_file*sizeof(struct epic_file));
	struct epic_file* pfile_metadata = (struct epic_file*)(data+file_metadata_offset);
	pfile_metadata->type = type;
	strcpy(pfile_metadata->filename, filename);
	pfile_metadata->size = 0;
	pfile_metadata->data = 0;
	pfile_metadata->last_data_cluster = 0;
	pfile_metadata->file_cluster = current_filecluster;
	pfile_metadata->file_offset = next_free_file*sizeof(struct epic_file);
	pfile_metadata->parent_cluster = 0;
	pfile_metadata->parent_offset = 0;
	pfile_metadata->inuse = 1;
	*pfiledata = pfile_metadata;
	next_free_file++;
	hdr->files_inroot++;
	printf("created %s in root with parent cluster %d\n", filename, pfile_metadata->parent_cluster);
	return 0;
}
int writefile(unsigned int cluster, unsigned int clusteroff, const char* src){
	if (!src||fat[cluster]==EPICFS_EOC||fat[cluster]==EPICFS_FC)
		return -1;
	FILE* srcfile = fopen(src, "rb");
	unsigned char* srcbuf = (unsigned char*)0x0;
	unsigned int srcsize = 0;
	unsigned int file_clusters = 0;
	if (!srcfile){
		printf("failed to open source file %s (%s)\n", src, strerror(errno));
		return -1;
	}
	fseek(srcfile,0,SEEK_END);
	srcsize = (unsigned int)ftell(srcfile);
	if (!srcsize)
		return -1;
	file_clusters = 1+((srcsize-1)/4096);
	rewind(srcfile);
	if (!srcsize)
		return 0;
	srcbuf = (unsigned char*)malloc(align_up(srcsize+4096-(srcsize%4096), 16));
	if (!srcbuf){
		printf("failed to allocate memory for source file (%s)\n", strerror(errno));
		fclose(srcfile);
		return -1;
	}
	unsigned int bytes_read = (unsigned int)fread((void*)srcbuf, 1, srcsize, srcfile);	
	fclose(srcfile);
	struct epic_file* pepic_file = (struct epic_file*)(data+(cluster*4096)+clusteroff+sizeof(struct epic_clusterhdr));
	struct epic_clusterhdr* pclusterhdr = (struct epic_clusterhdr*)(data+(cluster*4096));
	if (pclusterhdr->type==CLUSTER_INVALID){
		printf("invalid file!\n");
		free(srcbuf);
		return -1;
	}
	pepic_file->size = srcsize;
	unsigned int last_cluster = 0;
	for (unsigned int i = 0;i<file_clusters;i++){
		unsigned int new_cluster = allocate_cluster(CLUSTER_FILEDATA);
		if (!new_cluster){
			printf("failed to create new cluster\n");
			break;
		}
		if (!pepic_file->data)
			pepic_file->data = new_cluster;
		unsigned char* srcchunk = (unsigned char*)(srcbuf+(4096*i));
		unsigned int dt = srcsize%4096;
		unsigned int bytes_towrite = 4096;
		if (i==file_clusters-1&&dt)
			bytes_towrite = dt;
		unsigned int clusteroff = new_cluster*4096;
		memcpy((void*)(data+clusteroff), (const void*)srcchunk, bytes_towrite);
		pepic_file->last_data_cluster = new_cluster;
		if (last_cluster!=0){
			fat[last_cluster] = new_cluster;
		}
		last_cluster = new_cluster;
	}
	free(srcbuf);
	return 0;
}
int readfile(unsigned int cluster, unsigned int clusteroff, unsigned char* buf){
	if (!buf)
		return -1;
	unsigned int filemd_off = (cluster*4096)+sizeof(struct epic_clusterhdr)+clusteroff;
	struct epic_file* pfilemd = (struct epic_file*)(data+filemd_off);
	unsigned int current_cluster = pfilemd->data;
	unsigned int filedata_clusters = 1+(((pfilemd->size-1)/4096));
	for (unsigned int i = 0;i<filedata_clusters;i++){
		unsigned int toread = 4096;
		unsigned int dt = pfilemd->size%4096;
		if (i==filedata_clusters-1&&dt)
			toread = dt;
		memcpy((void*)(buf+(4096*i)), (const void*)(data+(4096*current_cluster)), toread);
		current_cluster = fat[current_cluster];
	}
	return 0;	
}
int adddir(const char* dirname){
	if (!dirname)
		return -1;
	DIR* dir = opendir(dirname);
	if (!dir)
		return -1;
	struct dirent* dirent = (struct dirent*)0x0;
	while ((dirent=readdir(dir))){
		char fullpath[256] = {0};
		char* fspath = (char*)fullpath+maindir_len+1;		
		snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname, dirent->d_name);
		if (dirent->d_type==DT_DIR&&*dirent->d_name!='.'&&dirent->d_name[1]!='.'){
			struct epic_file* pdirmd = (struct epic_file*)0x0;
			if (createfile(fullpath+maindir_len+1, &pdirmd, FILE_DIR)!=0)
				continue;
			adddir(fullpath);
			continue;
		}
		if (dirent->d_type!=DT_REG)
			continue;
		struct epic_file* pfilemd = (struct epic_file*)0x0;
		if (createfile(fspath, &pfilemd, FILE_REGULAR)!=0||!pfilemd){
			printf("failed to create file (%s)\n", fullpath);
			continue;
		}
		if (writefile(pfilemd->file_cluster, pfilemd->file_offset, fullpath)!=0){
			printf("failed to write to (%s)\n", fullpath);
			continue;
		}
	}
	closedir(dir);
	return 0;
}
int main(int argc, char** argv){
	if (argc<3)
		return -1;
	maindir = argv[1];
	char* outputfs = argv[2];
	char* strdrivesize = argv[3];
	if (!maindir||!outputfs||!strdrivesize)
		return -1;
	maindir_len = strlen(maindir);
	size_t drivesize = strtoull(strdrivesize, NULL, 0);
	size_t sizedt = drivesize%512;
	if (sizedt)
		drivesize+=(512-sizedt);	
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
	hdr->freelist_off = FS_RESERVED_SECTORS+8+(hdr->fat_size/512);
	hdr->freelist_size = drivesize/4096;
	hdr->freelist_size+=512-(hdr->freelist_size%512);
	hdr->data_off = FS_RESERVED_SECTORS+8+(hdr->fat_size/512)+(hdr->freelist_size/512);
	data = (unsigned char*)(fsbuf+4096+hdr->fat_size+hdr->freelist_size);
	freelist = (unsigned char*)(fsbuf+4096+hdr->fat_size);
	printf("%d\n", FS_RESERVED_SECTORS+(4096+hdr->fat_size)/512);
	fat_entries = (hdr->freelist_size);
	memset((void*)(fat), 0, hdr->fat_size);
	memset((void*)(freelist), 0, hdr->freelist_size);
	adddir(maindir);
	fat[hdr->fat_size/512] = EPICFS_EOC;
	fsused = drivesize;
	size_t reserved_bytes = 512*128;
	unsigned char* outbuf = (unsigned char*)malloc(drivesize+reserved_bytes);
	if (!outbuf){
		printf("failed to allocate memory for output buffer (%s)\n", strerror(errno));
		free(fsbuf);
		return -1;
	}
	memcpy((void*)(outbuf+reserved_bytes), (const void*)fsbuf, drivesize);
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
