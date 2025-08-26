#include <stdint.h>
#include "video.h"
#include "panic.h"
#include "interrupt.h"
#include "kernel.h"
#include "exception.h"
void exception_handler(uint32_t exception, uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t ebp, uint32_t esp, uint32_t eip, uint32_t cs, uint32_t flags){
	static const char* exception_map[] = {
		[EXCEPT_DIVBYZERO]="divide by zero",
		[EXCEPT_DEBUG]="debug", 
		[EXCEPT_NMI]="non maskable interrupt",
		[EXCEPT_BREAK]="breakpoint",
		[EXCEPT_OVERFLOW]="overflow",
		[EXCEPT_BRE]="bound range exceeded",
		[EXCEPT_INVALID_INSTRUCTION]="invalid instruction",
		[EXCEPT_DEV_NOT_AVALIBLE]="device not avalible",
		[EXCEPT_DOUBLE_FAULT]="double fault",
		[EXCEPT_INVALID_TSS]="invalid tss",
		[EXCEPT_SEGMENT_NOT_PRESENT]="segment not present",
		[EXCEPT_STACK_SEGMENT_FAULT]="stack segment fault",
		[EXCEPT_GENERAL_PROTECT_FAULT]="general protection fault",
		[EXCEPT_PAGE_FAULT]="page fault",
		[EXCEPT_FLOATING_POINT]="floating point",
		[EXCEPT_ALIGN_FAIL]="alignment fail",
		[EXCEPT_MACHINE_FAIL]="hardware failure",
		[EXCEPT_SIMD_FLOAT]="simd floating fail",
		[EXCEPT_VIRTUALIZATION]="virtualization",
		[EXCEPT_CONTROL_PROTECT]="control protection",
		[EXCEPT_HYPERVISOR_INJECT]="hypervisor inject",
		[EXCEPT_VMM_COMM]="vmm communication fail",
		[EXCEPT_SECURITY]="security fail",
	};
	__asm__ volatile("cli");
	clear();
	vga_set_color(VGA_COLOR_BRIGHT_WHITE, VGA_COLOR_BLUE);
	const char* exception_name = exception_map[exception];
	if (!exception_name)
		exception_name = "unknown";
	printf("%s exception triggered!\n", exception_name);
	printf("eip: %p     cs: %p     flags: %p\n", eip, cs, flags);
	printf("eax: %p     ebx: %p     ecx: %p\n", eax, ebx, ecx);
	printf("edx: %p     ebp: %p     esp: %p\n", edx, ebp, esp);
	for (unsigned int i = 0;i<64;i++){
		printf("0x%X, ", *(unsigned char*)(eip+i));
	}
	while (1){
		keyboard_interrupt();
	};
	return;
}
