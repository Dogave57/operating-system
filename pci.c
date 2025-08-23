#include "stdlib.h"
#include "video.h"
#include "kernel.h"
#include "pci.h"
unsigned char*** pci_buses = (unsigned char***)0x0;
unsigned char pci_initialized = 0;
uint32_t pci_read_dword_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset){
	uint32_t addr = pci_get_config(bus,dev,func,offset);
	uint32_t conf = 0;
	outb(0x0,0x0);
	outl(0xCF8, addr);
	outb(0x0,0x0);
	conf = inl(0xCFC);
	outl(0xCF8, 0x0);
	outl(0xCFC, 0x0);
	return conf;
}
uint16_t pci_read_word_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset){
	uint32_t addr = pci_get_config(bus,dev,func,offset);
	uint16_t conf = 0;
	outb(0x0,0x0);
	outl(0xCF8, addr);
	outb(0x0,0x0);
	conf = (uint16_t)(inl(0xCFC)>>((offset&2)*8)&0xFFFF);
	outl(0xCF8,0x0);
	outw(0xCFC,0x0);
	return conf;
}
uint8_t pci_read_byte_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset){
	uint32_t addr = pci_get_config(bus,dev,func,offset);
	uint8_t conf = 0;
	outb(0x0,0x0);
	outl(0xCF8, addr);
	outb(0x0,0x0);
	conf = (uint8_t)(inl(0xCFC)>>((offset%4)*8)&0xFF);
	outl(0xCF8,0x0);
	outb(0xCFC,0x0);
	return conf;
}
uint16_t pci_get_vendor(uint8_t bus, uint8_t dev, uint8_t func){
	return (uint16_t)pci_read_word_conf(bus,dev,func,0x0);
}
uint16_t pci_get_devid(uint8_t bus, uint8_t dev, uint8_t func){
	return (uint16_t)pci_read_word_conf(bus,dev,func,0x2);
}
uint8_t pci_get_class(uint8_t bus, uint8_t dev, uint8_t func){
	return (uint8_t)pci_read_byte_conf(bus,dev,func,0xB);
}
uint8_t pci_get_subclass(uint8_t bus, uint8_t dev, uint8_t func){
	return (uint8_t)pci_read_byte_conf(bus,dev,func,0xA);
}
uint8_t pci_get_progif(uint8_t bus, uint8_t dev, uint8_t func){
	return (uint8_t)pci_read_byte_conf(bus,dev,func,0x9);
}
int pci_get_bars(uint8_t bus, uint8_t dev, uint8_t func, struct pci_bar_data* bars){
	if (bars==(struct pci_bar_data*)0x0)
		return -1;
	for (unsigned int i = 0;i<6;i++){
		uint32_t val = pci_read_dword_conf(bus,dev,func,0x10+(i*4));
		struct pci_bar* bar_entry = (struct pci_bar*)(bars->pcibars+i);
		if (val&1){
			bar_entry->base = val&0xFFFFFFFC;
			bar_entry->barType = PCI_BAR_PORT;
			continue;
		}
		bar_entry->isX64 = val&(2);
		bar_entry->base = val&0xFFFFFFF0;
		bar_entry->barType = PCI_BAR_MMIO;	
	}
	return 0;
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
	pci_buses = (unsigned char***)kmalloc(PCI_BUSES_SIZE);
	if (pci_buses==(unsigned char***)0x0){
		printf("failed to allocate memory for pci bus data\n");
		return -1;
	}
	pci_initialized = 1;
	return 0;
}
int pci_deinit(void){
	if (!pci_initialized)
		return -1;
	kfree((void*)pci_buses);
	return 0;
}
int pci_device_exists(uint8_t bus, uint8_t dev, uint8_t func){
	if (pci_initialized==0)
		return 0;
	if (pci_buses[bus][dev][func])
		return 1;
	unsigned char exists = (pci_get_vendor(bus,dev,func)!=0xFFFF);
	pci_buses[bus][dev][func] = exists;
	return exists;
}
struct pci_device pci_get_device(uint8_t class, uint8_t subclass, uint8_t progif, uint8_t bus_max, uint8_t dev_max){
	if (!bus_max)
		bus_max = 255;
	if (!dev_max)
		dev_max = 32;
	struct pci_device ret = {0};
	for (unsigned int bus = 0;bus<bus_max;bus++){
		for (unsigned int dev = 0;dev<dev_max;dev++){
			for (unsigned int func = 0;func<8;func++){
				uint32_t vendor_id = pci_get_vendor(bus,dev,func);
				if (vendor_id==0xFFFF)
					continue;
				uint32_t dev_id = pci_get_devid(bus,dev,func);
				uint8_t dev_class = pci_get_class(bus,dev,func);
				uint8_t dev_subclass = pci_get_subclass(bus,dev,func);
				uint8_t dev_progif = pci_get_progif(bus,dev,func);
				if (dev_class!=class||dev_subclass!=subclass||(dev_progif!=progif&&progif!=0))
					continue;
				ret.bus = bus;
				ret.dev = dev;
				ret.func = func;
				ret.class = class;
				ret.subclass = subclass;
				ret.progif = progif;
				ret.vendor_id = vendor_id;
				ret.device_id = dev_id;
				return ret;
			}
		}
	}
	return ret;
}
