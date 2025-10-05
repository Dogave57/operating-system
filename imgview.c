#include "libsys.h"
#include "vim.h"
#include "stdlib.h"
int write_vim(unsigned char* pimg);
int write_vim(unsigned char* pimg){
	if (!pimg)
		return -1;
	struct vim_img* pimage = (struct vim_img*)pimg;
	if (!IS_VIM((unsigned char*)pimage->ident)){
		printf("Invalid vim image\n");
		return -1;
	}
	unsigned int pixelcnt = pimage->width*pimage->height;
	for (unsigned int i = 0;i<pixelcnt;i++){
		unsigned int palette_index = pimage->pixel_data[i];
		unsigned char color = pimage->palette[palette_index];
		unsigned int x = i%pimage->width;
		unsigned int y = i/pimage->width;
		sys_writecoord(x, y, color);
	}
	return 0;
}
int _start(char** argp, unsigned int argc){
	if (argc<2){
		sys_print("no image provided!\n");
		return -1;
	}
	unsigned int bootdrive = sys_getbootdrive();
	char* filename = argp[1];
	struct file* pfile = sys_openfile(bootdrive, filename);
	if (!pfile){
		sys_print("Invalid file\n");
		return -1;
	}
	unsigned int filesize = sys_getfilesize(pfile);
	if (!filesize){
		sys_print("Invalid image\n");
		sys_closefile(pfile);
		return -1;
	}
	unsigned char* imagedata = (unsigned char*)sys_kmalloc(filesize);
	if (!imagedata){
		sys_print("failed to allocate memory for image data\n");
		sys_closefile(pfile);
		return -1;
	}
	if (sys_readfile(pfile, imagedata)!=0){
		sys_print("failed to read image\n");
		sys_closefile(pfile);
		sys_kfree((void*)imagedata);
		return -1;
	}
	sys_closefile(pfile);
	struct vim_img* phdr = (struct vim_img*)imagedata;
	if ((phdr->width*phdr->height)>300000){
		printf("image too large!\n");
		sys_kfree((void*)imagedata);
		return -1;
	}
	write_vim((unsigned char*)imagedata);
	sys_kfree((void*)imagedata);
	return 0;
}

