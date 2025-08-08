struct fs_link{
	char filename[32];
	unsigned int linksize;
	unsigned int index;
	unsigned int parent;
	unsigned int child;
	unsigned int flink;
	unsigned int blink;
	char data[1];
};
