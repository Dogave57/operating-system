#ifndef _FILESYSTEM
#define _FILESYSTEM
#define FS_RESERVED_SECTORS 128
#define EPICFS_SIGNATURE (unsigned int)0x43495045 //EPIC
#define EPICFS_EOC (unsigned int)0xFFFFFFFF
#define EPICFS_FC (unsigned int)0x0
enum fsType{
	FS_NONE,
	FS_EPIC,
};
enum clusterType{
	CLUSTER_INVALID,
	CLUSTER_FILEDATA,
	CLUSTER_FILE,
};
enum fileType{
	FILE_INVALID,
	FILE_REGULAR,
	FILE_DIR,
};
struct epic_fshdr{
	unsigned int signature;
	unsigned int bytes_per_cluster;
	unsigned int fat_size;
	unsigned int freelist_size;
	unsigned int freelist_off;
	unsigned int data_off;
	unsigned int last_filemd_cluster;
	unsigned int files_inroot;
	unsigned int last_cluster;
}__attribute__((packed));
struct epic_clusterhdr{
	enum clusterType type;
	unsigned int cluster;
}__attribute__((packed));
struct epic_file{
	enum fileType type;
	char filename[16];
	unsigned int size;
	unsigned int data;
	unsigned int last_data_cluster;
	unsigned int file_cluster;
	unsigned int file_offset;
	unsigned int inuse;
	unsigned int parent_cluster;
	unsigned int parent_offset;
	unsigned char reserved[12];
}__attribute__((aligned(16)));
struct file{
	enum fsType fstype;
	enum fileType filetype;
	unsigned int len;
	unsigned int drive;
	unsigned int file_cluster;
	unsigned int file_offset;
	unsigned char reserved[16];
};
#endif
