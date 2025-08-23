#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "video.h"
#include "pci.h"
#include "usb.h"
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
	print("successfully got usb 2.0 bar data\n");
	return 0;	
}
int usb_init(void){
	print("initializing pci\n");
	if (!pci_init())
		print("successfully initialized pci\n");
	else
		print("failed to initialize pci\n");
	if (usb2_init()!=0){
		print("fialed to initialize usb 2.0\n");
	}
	return 0;
}

