#ifndef _PCI
#define _PCI
#define pci_get_config(bus, dev, func, offset)((uint32_t)1u<<31)|((uint32_t)bus<<16)|((uint32_t)dev<<11)|((uint32_t)func<<8)|((uint32_t)offset&0xFC)
#define PCI_VENDOR_INTEL 0x8086
#define PCI_VENDOR_AMD 0x1022
#define PCI_VENDOR_NVIDIA 0x10DE
#define PCI_VENDOR_VIA 0x1106
#define PCI_VENDOR_REALTEK 0x10EC
#define PCI_VENDOR_QEMU 0x1234
#define PCI_VENDOR_CIRRUS 0x1013
#define PCI_VENDOR_BROADCOM 0x14E4
extern unsigned char pci_buses[256][32];
uint32_t pci_read_dword_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint16_t pci_read_word_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint8_t pci_read_byte_conf(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint32_t pci_get_vendor(uint8_t bus, uint8_t dev);
uint32_t pci_get_devid(uint8_t bus, uint8_t dev);
const char* pci_get_vendorname(uint32_t vendorid);
int pci_init(void);
#endif
