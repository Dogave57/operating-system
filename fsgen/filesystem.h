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
	unsigned int data_off;
}__attribute__((packed));
struct epic_clusterhdr{
	enum clusterType type;
	unsigned int cluster;
}__attribute__((packed));
struct epic_file{
	struct epic_clusterhdr clusterhdr;
	enum fileType type;
	char filename[12];
	unsigned int size;
	unsigned int data;
}__attribute__((packed));
struct file{
	enum fsType fstype;
	enum fileType filetype;
	unsigned int len;
	unsigned int drive;
	unsigned int file_cluster;
	unsigned char reserved[16];
};
#endif
