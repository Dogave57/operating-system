#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "stdlib.h"
#include "panic.h"
#include "video.h"
#include "timer.h"
#include "pci.h"
#include "usb.h"
static struct usb2_queue_head usb2_base_qh = {0};
static struct usb2_queue_head* usb2_last_qh = (struct usb2_queue_head*)0x0;
static uint32_t* usb2_base = (uint32_t*)0x0;
static uint32_t* usb2_registers = (uint32_t*)0x0;
static uint32_t* usb2_usbcmd = (uint32_t*)0x0;
static uint32_t* usb2_status = (uint32_t*)0x0;
static uint32_t* usb2_async_list = (uint32_t*)0x0;
static uint32_t* usb2_portscn = (uint32_t*)0x0;
int usb2_add_qh(struct usb2_queue_head qh){
	struct usb2_queue_head* pqh = (struct usb2_queue_head*)kmalloc(sizeof(struct usb2_queue_head));
	if (!pqh)
		return -1;
	*pqh = qh;
	if (usb2_last_qh==(struct usb2_queue_head*)0x0){
		return -1;
	}
	usb2_last_qh->flink = (uint32_t)pqh;
	pqh->blink = (uint32_t)usb2_last_qh;
	usb2_last_qh = pqh;
	return 0;
}
int usb2_remove_qh(struct usb2_queue_head* qh){
	if (qh==(struct usb2_queue_head*)0x0)
		return -1;
	struct usb2_queue_head* flink = (struct usb2_queue_head*)qh->flink;	
	struct usb2_queue_head* blink = (struct usb2_queue_head*)qh->blink;
	if (flink!=(struct usb2_queue_head*)0x0)
		flink->blink = qh->blink;
	if (blink!=(struct usb2_queue_head*)0x0)
		blink->flink = qh->flink;
	return kfree((void*)qh);
}
int usb2_init(void){
	struct pci_device usb2_dev = pci_get_device(0x0C,0x03,0x20, 0x0, 0x0);
	struct pci_bar_data bar_data = {0};
	if (!usb2_dev.vendor_id||usb2_dev.vendor_id==0xFFFF){
		print("usb 2.0 controller does not exist\n");
		return -1;
	}
	printf("usb 2.0 controller vendor id: %x device id: %x\n", usb2_dev.vendor_id, usb2_dev.device_id);
	if (pci_get_bars(usb2_dev.bus, usb2_dev.dev, usb2_dev.func, &bar_data)!=0){
		print("failed to get usb 2.0 controller bar data\n");
		return -1;
	}	
	struct pci_bar usb2_bar = bar_data.pcibars[0];
	usb2_base = (uint32_t*)usb2_bar.base;
	uint32_t usb2_portcnt = *(usb2_base+1)&0xFF;
	uint32_t registers_off = *(usb2_base)&0xFF;
	usb2_registers = (uint32_t*)((unsigned char*)usb2_base+registers_off);
	usb2_usbcmd = (uint32_t*)(usb2_registers);
	usb2_status = (uint32_t*)(usb2_registers+0x1);
	usb2_async_list = (uint32_t*)(usb2_registers+0x18);
	usb2_portscn = (uint32_t*)(usb2_registers+0x10);
//	printf("usb 2.0 mmio address: %p\n", (void*)usb2_base);
	printf("port count: %d\n", usb2_portcnt);
	*usb2_status = 0xFFFFFFFF;
	*usb2_usbcmd|=0x1;
	while (*usb2_status==0xFFFFFFFF){};
	if ((*usb2_status)&(1<<2)){
		panic("usb2 controller transfer failed\n");
		return -1;
	}
	if ((*usb2_status)&(1<<5)){
		panic("usb2 controller reported system error\n");
		return -1;
	}
	for (unsigned int i = 0;i<usb2_portcnt;i++){
		uint32_t portval = usb2_portscn[i]&0xFF;
		unsigned char portspeed = 0;
		struct usb2_dev_desc dev_desc = {0};
		struct usb2_queue_head newqh = {0};
		if (!(portval&(1<<0)))
			continue;
		usb2_portscn[i]|=(1<<7);
		outb(0x0,0x0);
		portspeed = ((unsigned char*)(usb2_portscn+i))[9];
		printf("device connected to port %d with speed %d\n", i, portspeed);
		usb2_add_qh(newqh);
	}
	return 0;	
}
int usb2_deinit(void){
	struct usb2_queue_head* current_qh = (struct usb2_queue_head*)&usb2_base_qh;
	while (current_qh){
		usb2_remove_qh(current_qh);
		current_qh = (struct usb2_queue_head*)current_qh->flink;
	}
	return 0;
}
int usb_init(void){
	print("initializing pci\n");
	if (!pci_init())
		print("successfully initialized pci\n");
	else
		print("failed to initialize pci\n");
	if (usb2_init()!=0){
		print("failed to initialize usb 2.0\n");
	}
	return 0;
}
int usb_deinit(void){
	if (usb2_deinit()!=0){
		print("failed to deinitialize usb 2.0\n");
	}
	return 0;
}
