#ifndef _USB
#define _USB
struct usb2_transfer_desc;
struct usb2_dev_desc;
struct usb2_queue_head;
struct usb2_dev{
	struct usb2_dev* flink;
	struct usb2_dev* blink;
	struct usb2_queue_head* qh;
	struct usb2_dev_desc* dev_desc;
	uint32_t port;
};
struct usb2_transfer_desc{
	struct usb2_transfer_desc* flink;
	uint32_t token;
	uint32_t buffer[5];
	uint32_t reserved;
	struct usb2_transfer_desc* blink;
}__attribute__((packed));
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
	struct usb2_queue_head* flink;
	uint32_t endpoint_char;
	uint32_t endpoint_cap;
	uint32_t current_td_ptr;
	uint32_t next_td_ptr;
	struct usb2_queue_head* blink;
	uint32_t reserved[2];
	struct usb2_transfer_desc* first_td;
	struct usb2_transfer_desc* last_td;
}__attribute__((packed));
struct usb2_queue_head* usb2_add_qh(struct usb2_queue_head qh);
int usb2_remove_qh(struct usb2_queue_head* pqh);
int usb2_remove_all_qh(void);
struct usb2_transfer_desc* usb2_add_td(struct usb2_queue_head* pqh, struct usb2_transfer_desc td);
int usb2_remove_td(struct usb2_transfer_desc* ptd);
int usb2_remove_all_td(struct usb2_queue_head* pqh);
struct usb2_dev* usb2_register_dev(struct usb2_dev dev);
int usb2_unregister_dev(struct usb2_dev* pdev);
int usb2_remove_all_dev(void);
int usb2_init(void);
int usb_init(void);
int usb2_deinit();
int usb_deinit();
#endif
