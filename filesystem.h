#ifndef _FILESYSTEM
#define _FILESYSTEM
#include <stdint.h>
#include "kernel.h"
#define FS_RESERVED_SECTORS 128
struct FAT32_FS_INFO{
    uint8_t  jmpBoot[3];        // Jump instruction to boot code
    uint8_t  OEMName[8];        // OEM name
    uint16_t bytesPerSector;    // Usually 512
    uint8_t  sectorsPerCluster; // 1, 2, 4, 8, etc.
    uint16_t reservedSectorCount; // Reserved sectors before FAT
    uint8_t  numFATs;           // Usually 2
    uint16_t rootEntryCount;    // 0 for FAT32
    uint16_t totalSectors16;    // If 0, use totalSectors32
    uint8_t  media;             // Media descriptor
    uint16_t FATSize16;         // 0 for FAT32
    uint16_t sectorsPerTrack;
    uint16_t numHeads;
    uint32_t hiddenSectors;
    uint32_t totalSectors32;

    // FAT32-specific
    uint32_t FATSize32;         // Sectors per FAT
    uint16_t extFlags;
    uint16_t FSVersion;
    uint32_t rootCluster;       // Usually 2
    uint16_t FSInfo;            // Usually sector 1
    uint16_t backupBootSector;
    uint8_t  reserved[12];
    uint8_t  driveNumber;
    uint8_t  reserved1;
    uint8_t  bootSignature;
    uint32_t volumeID;
    uint8_t  volumeLabel[11];
    uint8_t  fileSystemType[8]; // "FAT32   "

}__attribute__((packed));
int read_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int bytespersector);
int write_sectors(unsigned int drive, uint32_t sector, uint8_t sectorcnt, uint16_t* buffer, unsigned int bytespersector);
int drive_getinfo(unsigned int drive, uint16_t* info);
struct highlow_64 drive_getsectors(unsigned int drive);
unsigned int getbootdrive(void);
#endif
