#ifndef _KERNEL
#define _KERNEL
void reboot(void);
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
#endif
