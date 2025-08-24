#ifndef _SMBIOS
#define _SMBIOS
#define SMBIOS_HEADER_BIOSINFO 0
#define SMBIOS_HEADER_SYSINFO 1
#define SMBIOS_HEADER_MOBOINFO 2
#define SMBIOS_HEADER_ENCLOSURE_INFO 3
#define SMBIOS_HEADER_CPU_INFO 4
#define SMBIOS_HEADER_CACHE_INFO 7
#define SMBIOS_HEADER_SYSTEM_SLOTS_INFO 9
#define SMBIOS_HEADER_PHYSICALMEM_ARRAY 16
#define SMBIOS_HEADER_MEM_DEV_INFO 17
#define SMBIOS_HEADER_ARRAY_MAPPED_ADDRESS 19
#define SMBIOS_HEADER_MEM_DEVICE_MAPPED_ADDRESS 20
#define SMBIOS_SYSBOOT_INFO 32
struct smbios_header{
	uint8_t type;
	uint8_t len;
	uint16_t handle;
}__attribute__((packed));
struct smbios_sysinfo{
	struct smbios_header header;
	uint8_t manufacturer_index;
	uint8_t product_index;
	uint8_t version_index;
	uint16_t uuid;
	uint8_t wakeup_type;
	uint8_t sku_number;
	uint8_t family;
}__attribute__((packed));
uint32_t get_smbios(void);
struct smbios_sysinfo* smbios_get_sysinfo(uint32_t smbios);
#endif
