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
	uint8_t manufacturer_name;
	uint8_t product_name;
	uint8_t version_name;
	uint8_t serial_name;
	uint8_t uuid[16];
	uint8_t wakeup_type;
	uint8_t sku_name;
	uint8_t family_name;
	char padding0[5];
}__attribute__((packed));
uint32_t get_smbios(void);
struct smbios_sysinfo* smbios_get_sysinfo(void);
char* smbios_get_string(char* blob, unsigned int index);
char* smbios_get_manufacturer_name(void);
char* smbios_get_product_name(void);
char* smbios_get_version_name(void);
char* smbios_get_serial_name(void);
uint8_t* smbios_get_uuid(void);
char* smbios_get_sku_name(void);
char* smbios_get_family_name(void);
#endif

