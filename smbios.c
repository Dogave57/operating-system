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
struct smbios_moboinfo* moboinfo = (struct smbios_moboinfo*)0x0;
struct smbios_cpuinfo* cpuinfo = (struct smbios_cpuinfo*)0x0;
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
struct smbios_moboinfo* smbios_get_moboinfo(void){
	if (moboinfo)
		return moboinfo;
	if (!smbios)
		smbios = get_smbios();
	moboinfo = (struct smbios_moboinfo*)smbios_get_entry(SMBIOS_HEADER_MOBOINFO);
	return moboinfo;
}
struct smbios_cpuinfo* smbios_get_cpuinfo(void){
	if (cpuinfo)
		return cpuinfo;
	if (!smbios)
		smbios = get_smbios();
	cpuinfo = (struct smbios_cpuinfo*)smbios_get_entry(SMBIOS_HEADER_CPU_INFO);
	return cpuinfo;
}
struct smbios_meminfo* smbios_get_meminfo(void){
	if (meminfo)
		return meminfo;
	if (!smbios)
		smbios = get_smbios();
	meminfo = (struct smbios_meminfo*)smbios_get_entry(SMBIOS_HEADER_MEMINFO);
	return meminfo;
}
char* smbios_get_string(void* entry, unsigned int index){
	if (!entry)
		return (char*)0x0;
	struct smbios_header* hdr = (struct smbios_header*)entry;
	char* blob = (char*)((unsigned char*)entry+hdr->len);
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
