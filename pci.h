#ifndef _PCI
#define _PCI
#define pci_get_config(bus, dev, func, offset)(((uint32_t)bus<<16)|((uint32_t)dev<<11)|((uint32_t)func<<8)|((uint32_t)offset&0xFC|((uint32_t)0x80000000)))
#define PCI_BUSES_SIZE 256*32*8
#define PCI_VENDOR_INTEL 0x8086
#define PCI_VENDOR_AMD 0x1022
#define PCI_VENDOR_NVIDIA 0x10DE
#define PCI_VENDOR_VIA 0x1106
#define PCI_VENDOR_REALTEK 0x10EC
#define PCI_VENDOR_QEMU 0x1234
#define PCI_VENDOR_CIRRUS 0x1013
#define PCI_VENDOR_BROADCOM 0x14E4
enum pciBar{
	PCI_BAR_NONE,
	PCI_BAR_MMIO,
	PCI_BAR_PORT,
};
struct pci_bar{
	uint32_t base;
	enum pciBar barType;
	unsigned char isX64;
};
struct pci_bar_data{
	struct pci_bar pcibars[6];
};
extern unsigned char*** pci_buses;
uint32_t pci_read_dword_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint16_t pci_read_word_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint8_t pci_read_byte_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint16_t pci_get_vendor(uint8_t bus, uint8_t dev, uint8_t func);
uint16_t pci_get_devid(uint8_t bus, uint8_t dev, uint8_t func);
uint8_t pci_get_class(uint8_t bus, uint8_t dev, uint8_t func);
uint8_t pci_get_subclass(uint8_t bus, uint8_t dev, uint8_t func);
uint8_t pci_get_progif(uint8_t bus, uint8_t dev, uint8_t func);
int pci_get_bars(uint8_t bus, uint8_t dev, uint8_t func, struct pci_bar_data* bars);
const char* pci_get_vendorname(uint32_t vendorid);
int pci_init(void);
int pci_deinit(void);
int pci_device_exists(uint8_t bus, uint8_t dev, uint8_t func);
#endif
