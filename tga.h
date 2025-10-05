#ifndef _TGA
#define _TGA
#define TGA_PALETTE_SIZE (256*3)
struct tga_hdr{
	uint8_t imageid;
	uint8_t colormap_type;
	uint8_t image_type;
	uint16_t colormap_firstentry;
	uint16_t colormap_len;
	uint8_t colormap_entrysize;
	uint16_t x_origin;
	uint16_t y_origin;
	uint16_t width;
	uint16_t height;
	uint8_t pixel_depth;
	uint8_t image_desc;
};
struct tga_pixel{
	uint8_t hdr;
	uint8_t pixel;
};
#endif
