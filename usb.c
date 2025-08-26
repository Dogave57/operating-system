#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "stdlib.h"
#include "panic.h"
#include "video.h"
#include "timer.h"
#include "pci.h"
#include "usb.h"
static struct usb2_queue_head* usb2_base_qh = (struct usb2_queue_head*)0x0;
static struct usb2_queue_head* usb2_last_qh = (struct usb2_queue_head*)0x0;
static struct usb2_dev* usb2_base_dev = (struct usb2_dev*)0x0;
static struct usb2_dev* usb2_last_dev = (struct usb2_dev*)0x0;
static uint32_t* usb2_base = (uint32_t*)0x0;
static uint32_t* usb2_registers = (uint32_t*)0x0;
static uint32_t* usb2_usbcmd = (uint32_t*)0x0;
static uint32_t* usb2_status = (uint32_t*)0x0;
static struct usb2_queue_head* usb2_async_list = (struct usb2_queue_head*)0x0;
static uint32_t* usb2_portscn = (uint32_t*)0x0;
struct usb2_queue_head* usb2_add_qh(struct usb2_queue_head qh){
	struct usb2_queue_head* pqh = (struct usb2_queue_head*)kmalloc(sizeof(struct usb2_queue_head));
	if (!pqh)
		return pqh;
	*pqh = qh;
	if (!usb2_last_qh){
		usb2_base_qh = pqh;
		usb2_last_qh = pqh;
		*usb2_async_list = qh;
	}
	usb2_last_qh->flink = pqh;
	pqh->blink = usb2_last_qh;
	usb2_last_qh = pqh;
	return pqh;
}
int usb2_remove_qh(struct usb2_queue_head* pqh){
	if (!pqh)
		return -1;
	if (pqh->flink)
		pqh->flink->blink = pqh->blink;
	if (pqh->blink)
		pqh->blink->flink = pqh->flink;
	usb2_remove_all_td(pqh);
	return 0;
}
int usb2_remove_all_qh(void){
	if (!usb2_base_qh)
		return -1;
	struct usb2_queue_head* current_qh = (struct usb2_queue_head*)usb2_base_qh;
	while (current_qh){
		usb2_remove_all_td(current_qh);
		current_qh=current_qh->flink;
	}
	return 0;
}
struct usb2_transfer_desc* usb2_add_td(struct usb2_queue_head* pqh, struct usb2_transfer_desc td){
	if (!pqh)
		return (struct usb2_transfer_desc*)0x0;
	struct usb2_transfer_desc* ptd = (struct usb2_transfer_desc*)kmalloc(sizeof(struct usb2_transfer_desc));
	if (!ptd)
		return ptd;
	if (!pqh->last_td){
		pqh->last_td = ptd;
		pqh->first_td = ptd;
		return ptd;
	}
	pqh->last_td->flink = ptd;
	ptd->blink = pqh->last_td;
	pqh->last_td = ptd;
	return ptd;
}
int usb2_remove_td(struct usb2_transfer_desc* ptd){
	if (!ptd)
		return -1;
	if (ptd->flink)
		ptd->flink->blink = ptd->blink;
	if (ptd->blink)
		ptd->blink->flink = ptd->flink;
	return 0;
}
int usb2_remove_all_td(struct usb2_queue_head* pqh){
	if (!pqh)
		return -1;
	pqh->next_td_ptr = 0;
	struct usb2_transfer_desc* current_ptd = (struct usb2_transfer_desc*)pqh->first_td;
	while (current_ptd){
		struct usb2_transfer_desc* flink = current_ptd->flink;
		current_ptd->flink = (struct usb2_transfer_desc*)0x0;
		current_ptd->blink = (struct usb2_transfer_desc*)0x0;
		kfree(current_ptd);
		current_ptd=flink;
	}
	return 0;
}
struct usb2_dev* usb2_register_dev(struct usb2_dev dev){
	struct usb2_dev* pdev = (struct usb2_dev*)kmalloc(sizeof(struct usb2_dev));
	if (!pdev)
		return pdev;
	*pdev = dev;
	if (!usb2_last_dev){
		usb2_last_dev = pdev;
		usb2_base_dev = pdev;
		return pdev;
	}
	usb2_last_dev->flink = pdev;
	pdev->blink = usb2_last_dev;
	usb2_last_dev = pdev;
	return pdev;
}
int usb2_unregister_dev(struct usb2_dev* pdev){
	if (!pdev)
		return -1;
	if (pdev->flink)
		pdev->flink->blink = pdev->blink;
	if (pdev->blink)
		pdev->blink->flink = pdev->flink;
	kfree(pdev);
	return 0;
}
int usb2_remove_all_dev(void){
	if (!usb2_base_dev)
		return -1;
	struct usb2_dev* current_dev = usb2_base_dev;
	while (current_dev){
		struct usb2_dev* flink = current_dev->flink;
		kfree(flink);
		current_dev=flink;
	}
	return 0;
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
	usb2_async_list = (struct usb2_queue_head*)(usb2_registers+0x18);
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
		struct usb2_transfer_desc out_td = {0};
		struct usb2_dev dev = {0};
		struct usb2_queue_head* pnewqh = (struct usb2_queue_head*)0x0;
		struct usb2_transfer_desc* pout_td = (struct usb2_transfer_desc*)0x0;
		struct usb2_dev* pnew_dev = (struct usb2_dev*)0x0;
		if (!(portval&(1<<0)))
			continue;
		usb2_portscn[i]|=(1<<7);
		outb(0x0,0x0);
		portspeed = ((unsigned char*)(usb2_portscn+i))[9];
		//printf("device connected to port %d with speed %d\n", i, portspeed);
		pnewqh = usb2_add_qh(newqh);
		if (!pnewqh){
			printf("failed to create new queue head\n");
			return -1;	
		}
		out_td.token |= 0xb00<<0;
		out_td.token |= 3<<2;
		out_td.token |= 1<<7;
		out_td.token |= 64<<16;
		out_td.token |= 1<<31;
		pout_td = usb2_add_td(pnewqh, out_td);
		if (!pout_td){
			printf("failed to add output transfer descn\n");
			return -1;
		}
		dev.port = i;
		dev.qh = pnewqh;
		pnew_dev = usb2_register_dev(dev);
		if (!pnew_dev){
			printf("failed to register new device\n");
			return -1;
		}
	}
	struct usb2_dev* current_dev = (struct usb2_dev*)usb2_base_dev;
	while (current_dev){
		printf("device found at port: %d with qh %p\n", current_dev->port, current_dev->qh);
		current_dev=current_dev->flink;
	}
	return 0;	
}
int usb2_deinit(void){
	usb2_remove_all_qh();	
	usb2_remove_all_dev();
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
