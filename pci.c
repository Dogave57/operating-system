#include "video.h"
#include "kernel.h"
#include "pci.h"
unsigned char pci_buses[256][32] = {0};
unsigned char pci_initialized = 0;
uint32_t pci_read_dword_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset){
	uint32_t addr = pci_get_config(bus,dev,func,offset);
	uint32_t conf = 0;
	outl(0xCF8, addr);
	outb(0x0,0x0);
	conf = inl(0xCFC);
	outb(0x0,0x0);
	return conf;
}
uint16_t pci_read_word_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset){
	uint32_t addr = (1u<< 31)|((uint32_t)bus<<16)|((uint32_t)dev<<11)|((uint32_t)func<<8)|((uint32_t)offset&0xFC);
	uint16_t conf = 0;
	outl(0xCF8, addr);
	outb(0x0,0x0);
	conf = inw(0xCFC);
	outb(0x0,0x0);
	return conf;
}
uint8_t pci_read_byte_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset){
	uint32_t addr = (1u<< 31)|((uint32_t)bus<<16)|((uint32_t)dev<<11)|((uint32_t)func<<8)|((uint32_t)offset&0xFC);
	uint8_t conf = 0;
	outl(0xCF8, addr);
	outb(0x0,0x0);
	conf = inb(0xCFC);
	outb(0x0,0x0);
	return conf;
}
uint32_t pci_get_vendor(uint8_t bus, uint8_t dev){
	return pci_read_word_conf(bus,dev,0,0);
}
uint32_t pci_get_devid(uint8_t bus, uint8_t dev){
	return pci_read_word_conf(bus,dev,0x0,0x2);
}
const char* pci_get_vendorname(uint32_t vendorid){
	const const char* vendorid_mapping[]={
		[PCI_VENDOR_INTEL]="intel",
		[PCI_VENDOR_AMD]="amd",
		[PCI_VENDOR_NVIDIA]="nvidia",
		[PCI_VENDOR_VIA]="via",
		[PCI_VENDOR_REALTEK]="realtek",
		[PCI_VENDOR_QEMU]="qemu",
		[PCI_VENDOR_CIRRUS]="cirrus logic",
		[PCI_VENDOR_BROADCOM]="broadcom",
	};
	return vendorid_mapping[vendorid];
}
int pci_init(void){
	if (pci_initialized!=0)
		return -1;
	for (unsigned int bus = 0;bus<256;bus++){
		for (unsigned int dev = 0;dev<31;dev++){
			uint32_t vendor_id = pci_get_vendor(bus, dev);
			uint32_t device_id = pci_get_devid(bus, dev);
			const char* vendor_name = (const char*)0x0;
			if (vendor_id==0xFFFF)
				continue;
			vendor_name = pci_get_vendorname(vendor_id);
			printf("pci device at bus %d device %d with device id: %x\n", bus, dev, device_id);
			if (vendor_name!=(const char*)0x0)
				printf("vendor name: %s\n", vendor_name);
			else
				printf("unknown vendor %x\n", vendor_id);
		}
	}
	return 0;
}
int pci_get_device(void){

	return 0;
}
