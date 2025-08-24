#include "kernel.h"
#include "video.h"
#include "memory.h"
#include "smbios.h"
uint32_t smbios = 0;
char* manufacturer_str = (char*)0x0;
char* product_str = (char*)0x0;
char* version_str = (char*)0x0;
char* serial_str = (char*)0x0;
struct smbios_biosinfo* biosinfo = (struct smbios_biosinfo*)0x0;
struct smbios_sysinfo* sysinfo = (struct smbios_sysinfo*)0x0;
struct smbios_meminfo* meminfo = (struct smbios_meminfo*)0x0;
uint32_t get_smbios(void){
	if (smbios!=0)
		return smbios;
	for (uint32_t* i = (uint32_t*)0xF0000;i<(uint32_t*)0xFFFFF;i++){
		if (*i!=0x5F4D535F)
			continue;
		smbios = (uint32_t)i;
		return smbios;
	}
	return 0x0;
}
uint32_t smbios_get_entry(uint32_t type){
	if (!smbios)
		smbios = get_smbios();
	uint8_t* addr = (uint8_t*)smbios;
	while (1){
		struct smbios_header* header = (struct smbios_header*)addr;
		if (header->type==127)
			break;
		if (header->type!=type){
			addr+=header->len;
			for (;*(uint16_t*)addr;addr++){};
			addr+=2;
			continue;
		}
		return (uint32_t)addr;
	}
	return 0;
}
struct smbios_biosinfo* smbios_get_biosinfo(void){
	if (biosinfo)
		return biosinfo;
	if (!smbios)
		smbios = get_smbios();
	biosinfo = (struct smbios_biosinfo*)smbios_get_entry(SMBIOS_HEADER_BIOSINFO);
	return biosinfo;
}
struct smbios_sysinfo* smbios_get_sysinfo(void){
	if (sysinfo)
		return sysinfo;
	if (!smbios)
		smbios = get_smbios();
	sysinfo = (struct smbios_sysinfo*)smbios_get_entry(SMBIOS_HEADER_SYSINFO);
	return sysinfo;
}
struct smbios_meminfo* smbios_get_meminfo(void){
	if (meminfo)
		return meminfo;
	if (!smbios)
		smbios = get_smbios();
	meminfo = (struct smbios_meminfo*)smbios_get_entry(SMBIOS_HEADER_MEMINFO);
	return meminfo;
}
char* smbios_get_string(char* blob, unsigned int index){
	if (blob==(char*)0x0)
		return (char*)0x0;
	unsigned int strindex = 0;
	unsigned int strstart = 0;
	for (unsigned int i = 0;;i++){
		if (blob[i]!=0)
			continue;
		strindex++;
		if (strindex==index){
			return (char*)blob+strstart;
		}
		strstart = i+1;
		if (blob[i+1]==0)
			break;
	}
	return (char*)0x0;
}
char* smbios_get_bios_vendor_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!biosinfo)
		biosinfo = smbios_get_biosinfo();
	char* strsect = (char*)0x0;
	if (!biosinfo)
		return (char*)0x0;
	strsect = (char*)((char*)biosinfo+biosinfo->header.len);
	return (char*)smbios_get_string(strsect, biosinfo->vendor_name);
}
char* smbios_get_bios_version_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!biosinfo)
		biosinfo = smbios_get_biosinfo();
	char* strsect = (char*)0x0;
	if (!biosinfo)
		return (char*)0x0;
	strsect = (char*)((char*)biosinfo+biosinfo->header.len);
	return (char*)smbios_get_string(strsect, biosinfo->version_name);
}
char* smbios_get_bios_release_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!biosinfo)
		biosinfo = smbios_get_biosinfo();
	char* strsect = (char*)0x0;
	if (!biosinfo)
		return (char*)0x0;
	strsect = (char*)((char*)biosinfo+biosinfo->header.len);
	return (char*)smbios_get_string(strsect, biosinfo->release_name);
}
char* smbios_get_sys_manufacturer_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!sysinfo)
		sysinfo = smbios_get_sysinfo();
	char* strsect = (char*)0x0;
	if (sysinfo==(struct smbios_sysinfo*)0x0)
		return (char*)0x0;
	strsect = (char*)((char*)sysinfo+sysinfo->header.len);
	return (char*)smbios_get_string(strsect, sysinfo->manufacturer_name);
}
char* smbios_get_sys_product_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!sysinfo)
		sysinfo = smbios_get_sysinfo();
	char* strsect = (char*)0x0;
	if (sysinfo==(struct smbios_sysinfo*)0x0)
		return (char*)0x0;
	strsect = (char*)((char*)sysinfo+sysinfo->header.len);
	return (char*)smbios_get_string(strsect, sysinfo->product_name);
}
char* smbios_get_sys_version_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!sysinfo)
		sysinfo = smbios_get_sysinfo();
	char* strsect = (char*)0x0;
	if (sysinfo==(struct smbios_sysinfo*)0x0)
		return (char*)0x0;
	strsect = (char*)((char*)sysinfo+sysinfo->header.len);
	return (char*)smbios_get_string(strsect, sysinfo->version_name);
}
char* smbios_get_sys_serial_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!sysinfo)
		sysinfo = smbios_get_sysinfo();
	char* strsect = (char*)0x0;
	if (sysinfo==(struct smbios_sysinfo*)0x0)
		return (char*)0x0;
	strsect = (char*)((char*)sysinfo+sysinfo->header.len);
	return (char*)smbios_get_string(strsect, sysinfo->serial_name);
}
uint8_t* smbios_sys_get_uuid(void){
	if (!smbios)
		smbios = get_smbios();
	if (!sysinfo)
		sysinfo = smbios_get_sysinfo();
	return (uint8_t*)sysinfo->uuid;
}
char* smbios_get_sys_sku_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!sysinfo)
		sysinfo = smbios_get_sysinfo();
	char* strsect = (char*)0x0;
	if (sysinfo==(struct smbios_sysinfo*)0x0)
		return (char*)0x0;
	strsect = (char*)((char*)sysinfo+sysinfo->header.len);
	return (char*)smbios_get_string(strsect, sysinfo->sku_name);
}
char* smbios_get_sys_family_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!sysinfo)
		sysinfo = smbios_get_sysinfo();
	char* strsect = (char*)0x0;
	if (sysinfo==(struct smbios_sysinfo*)0x0)
		return (char*)0x0;
	strsect = (char*)((char*)sysinfo+sysinfo->header.len);
	return (char*)smbios_get_string(strsect, sysinfo->family_name);
}
char* smbios_get_memory_manufacturer_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!meminfo)
		meminfo = smbios_get_meminfo();
	char* strsect = (char*)0x0;
	if (!meminfo)
		return (char*)0x0;
	strsect = (char*)((char*)meminfo+meminfo->header.len);
	return (char*)smbios_get_string(strsect, meminfo->manufacturer_name);
}
char* smbios_get_memory_serial_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!meminfo)
		meminfo = smbios_get_meminfo();
	char* strsect = (char*)0x0;
	if (!meminfo)
		return (char*)0x0;
	strsect = (char*)((char*)meminfo+meminfo->header.len);
	return (char*)smbios_get_string(strsect, meminfo->serial_name);
}
char* smbios_get_memory_asset_tag_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!meminfo)
		meminfo = smbios_get_meminfo();
	char* strsect = (char*)0x0;
	if (!meminfo)
		return (char*)0x0;
	strsect = (char*)((char*)meminfo+meminfo->header.len);
	return (char*)smbios_get_string(strsect, meminfo->asset_tag_name);
}
char* smbios_get_memory_partnum_name(void){
	if (!smbios)
		smbios = get_smbios();
	if (!meminfo)
		meminfo = smbios_get_meminfo();
	char* strsect = (char*)0x0;
	if (!meminfo)
		return (char*)0x0;
	strsect = (char*)((char*)meminfo+meminfo->header.len);
	return (char*)smbios_get_string(strsect, meminfo->partnum_name);
}
