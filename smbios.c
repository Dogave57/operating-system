#include "kernel.h"
#include "video.h"
#include "memory.h"
#include "smbios.h"
uint32_t smbios = 0;
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
struct smbios_sysinfo* smbios_get_sysinfo(uint32_t smbios){
	unsigned char* addr = (unsigned char*)smbios;
	while (1){
		struct smbios_header* header = (struct smbios_header*)addr;
		if (header->type==127)
			break;
//		printf("type: %d |", header->type);
		if (header->type!=SMBIOS_HEADER_SYSINFO){
			addr+=header->len;
			while (*(uint16_t*)addr!=0)
				addr++;
			addr+=2;
			continue;
		}
		return (struct smbios_sysinfo*)(addr);
	}	
	return (struct smbios_sysinfo*)0x0;
}
