#ifndef _VIM
#define _VIM
#define IS_VIM(x)((*(x)=='V')&&*(x+1)=='I'&&*(x+2)=='M'&&*(x+3)==0)
struct vim_img{
	unsigned char ident[4];
	uint32_t width;
	uint32_t height;
	uint8_t compression;
	uint8_t palette[256];
	uint8_t pixel_data[];
}__attribute__((packed));
#endif
