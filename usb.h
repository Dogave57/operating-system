#ifndef _USB
#define _USB
struct usb2_dev_desc{
	unsigned char len;
	unsigned char descType;
	uint16_t usb_vers;
	unsigned char devclass;
	unsigned char dev_subclass;
	unsigned char dev_protocol;
	unsigned char max_packetsize;
	uint16_t vendor_id;
	uint16_t dev_id;
	uint16_t dev_release;
	unsigned char manufacturer_strindex;
	unsigned char product_index;	
	unsigned char serial_index;
	unsigned char num_configs;
}__attribute__((packed));
struct usb2_queue_head{
	uint32_t flink;
	uint32_t endpoint_char;
	uint32_t endpoint_cap;
	uint32_t current_td_ptr;
	uint32_t next_td_ptr;
	uint32_t blink;
	uint32_t reserved[2];
}__attribute__((packed));
struct usb2_transfer_desc{
	uint32_t flink;
	uint32_t token;
	uint32_t buffer[5];
	uint32_t reserved;
}__attribute__((packed));
int usb2_add_qh(struct usb2_queue_head qh);
int usb2_remove_qh(struct usb2_queue_head* qh);
int usb2_init(void);
int usb_init(void);
int usb2_deinit();
int usb_deinit();
#endif
