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
#define SMBIOS_HEADER_MEMINFO 17
#define SMBIOS_HEADER_ARRAY_MAPPED_ADDRESS 19
#define SMBIOS_HEADER_MEM_DEVICE_MAPPED_ADDRESS 20
#define SMBIOS_SYSBOOT_INFO 32
struct smbios_header{
	uint8_t type;
	uint8_t len;
	uint16_t handle;

}__attribute__((packed));
struct smbios_biosinfo{
	struct smbios_header header;
	uint8_t vendor_name;
	uint8_t version_name;
	uint16_t bios_segment;
	uint8_t release_name;
	uint8_t romsize;
	uint64_t characteristics;
	uint8_t characteristics_extension[2];
	uint8_t systemBiosMajorRelease;
	uint8_t systemBiosMinorRelease;
	uint8_t embeddedControllerFirmwareMajorRelease;
	uint8_t embeddedControllerFirmwareMinorRelease;
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
struct smbios_moboinfo{
	struct smbios_header header;
	uint8_t manufacturer_name;
	uint8_t product_name;
	uint8_t version_name;
	uint8_t serial_name;
	uint8_t asset_tag_name;
	uint8_t feature_flags;
	uint8_t location_in_chassis_name;
	uint16_t chassis_handle;
	uint8_t boardType;
	uint8_t containedObjHandles_cnt;
	uint16_t contained_obj_handles[];
}__attribute__((packed));
struct smbios_cpuinfo{
	struct smbios_header header;
	uint8_t socket_name;
	uint8_t processorType;
	uint8_t processorFamily;
	uint8_t manufacturer_name;
	uint64_t processorId;
	uint8_t version_name;
	uint8_t voltage;
	uint16_t external_clock;
	uint16_t max_speed;
	uint16_t current_speed;
	uint8_t status;
	uint8_t upgradeMethod;
	uint16_t L1cacheHandle;
	uint16_t L2cacheHandle;
	uint16_t L3cacheHandle;
	uint8_t serial_name;
	uint8_t asset_tag_name;
	uint8_t partnum_name;
	uint8_t corecnt;
	uint8_t smpEnabled;
	uint8_t threadcnt;
	uint16_t processorCharacteristics;
}__attribute__((packed));
struct smbios_meminfo{
	struct smbios_header header;
	uint16_t phsicalmem_array_handle;
	uint16_t memory_err_infohandle;
	uint16_t total_data_width;
	uint16_t data_width;
	uint16_t size;
	uint8_t form_factor;
	uint8_t device_set;
	uint8_t device_locator;
	uint8_t bank_locator;
	uint8_t memoryType;
	uint16_t typeDetail;
	uint16_t speed;
	uint8_t manufacturer_name;
	uint8_t serial_name;
	uint8_t asset_tag_name;
	uint8_t partnum_name;
}__attribute__((packed));
uint32_t get_smbios(void);
uint32_t smbios_get_entry(uint32_t type);
struct smbios_biosinfo* smbios_get_biosinfo(void);
struct smbios_sysinfo* smbios_get_sysinfo(void);
struct smbios_moboinfo* smbios_get_moboinfo(void);
struct smbios_cpuinfo* smbios_get_cpuinfo(void);
struct smbios_meminfo* smbios_get_meminfo(void);
char* smbios_get_string(char* blob, unsigned int index);
char* smbios_get_bios_vendor_name(void);
char* smbios_get_bios_version_name(void);
char* smbios_get_bios_release_name(void);
char* smbios_get_sys_manufacturer_name(void);
char* smbios_get_sys_product_name(void);
char* smbios_get_sys_version_name(void);
char* smbios_get_sys_serial_name(void);
uint8_t* smbios_get_sys_uuid(void);
char* smbios_get_sys_sku_name(void);
char* smbios_get_sys_family_name(void);
char* smbios_get_mobo_manufacturer_name(void);
char* smbios_get_mobo_product_name(void);
char* smbios_get_mobo_version_name(void);
char* smbios_get_mobo_serial_name(void);
char* smbios_get_asset_tag_name(void);
char* smbios_get_mobo_chassis_location_name(void);
char* smbios_get_cpu_socket_name(void);
char* smbios_get_cpu_manufacturer_name(void);
char* smbios_get_cpu_version_name(void);
char* smbios_get_cpu_serial_name(void);
char* smbios_get_cpu_asset_tag_name(void);
char* smbios_get_cpu_partnum_name(void);
char* smbios_get_memory_manufacturer_name(void);
char* smbios_get_memory_serial_name(void);
char* smbios_get_memory_asset_tag_name(void);
char* smbios_get_memory_partnum_name(void);
#endif

