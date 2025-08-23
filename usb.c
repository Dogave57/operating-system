#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "video.h"
#include "pci.h"
#include "usb.h"
int usb_init(void){
	print("initializing pci\n");
	if (!pci_init())
		print("successfully initialized pci\n");
	else
		print("failed to initialize pci\n");
	return 0;
}

