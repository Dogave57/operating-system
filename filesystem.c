#include <stdint.h>
#include "bootloader.h"
#include "filesystem.h"
int read_sector(unsigned int drive, unsigned int sector, unsigned int sectorcnt, void* buffer){
	if (!buffer)
		return -1;
	
	return 0;
}
unsigned int getbootdrive(void){
	struct bootloader_args* blargs = (struct bootloader_args*)0x2000;
	return blargs->bootdrive;
}
