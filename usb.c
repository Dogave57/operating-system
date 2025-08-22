#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "video.h"
#include "pci.h"
#include "usb.h"
int usb_init(void){
	printf("implementing usb...\n");
	pci_init();
	return 0;
}

