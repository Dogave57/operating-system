#include "kernel.h"
#include "video.h"
#include "memory.h"
#include "smbios.h"
uint32_t smbios = 0;
char* manufacturer_str = (char*)0x0;
char* product_str = (char*)0x0;
char* version_str = (char*)0x0;
char* serial_str = (char*)0x0;
struct smbios_sysinfo* sysinfo = (struct smbios_sysinfo*)0x0;
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
struct smbios_sysinfo* smbios_get_sysinfo(void){
	if (sysinfo)
		return sysinfo;
	if (!smbios)
		smbios = get_smbios();
	unsigned char* addr = (unsigned char*)smbios;
	while (1){
		struct smbios_header* header = (struct smbios_header*)addr;
		if (header->type==127)
			break;
		if (header->type!=SMBIOS_HEADER_SYSINFO){
			addr+=header->len;
			while (*(uint16_t*)addr!=0)
				addr++;
			addr+=2;
			continue;
		}
		sysinfo = (struct smbios_sysinfo*)addr;
		return sysinfo;
	}	
	return (struct smbios_sysinfo*)0x0;
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
char* smbios_get_manufacturer_name(void){
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
char* smbios_get_product_name(void){
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
char* smbios_get_version_name(void){
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
char* smbios_get_serial_name(void){
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
uint8_t* smbios_get_uuid(void){
	if (!smbios)
		smbios = get_smbios();
	if (!sysinfo)
		sysinfo = smbios_get_sysinfo();
	return (uint8_t*)sysinfo->uuid;
}
char* smbios_get_sku_name(void){
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
char* smbios_get_family_name(void){
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
