global default_master_isr
global default_slave_isr
global timer_isr
global keyboard_isr
global mouse_isr
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr28
global isr29
global isr30
global syscall_isr
extern timer_interrupt
extern keyboard_interrupt
extern mouse_interrupt
extern printf
extern print
extern putchar
extern puthex
extern clear
extern openfile
extern createfile
extern deletefile
extern readfile
extern writefile
extern renamefile
extern getfilesize
extern closefile
extern kmalloc
extern kfree
extern exception_handler
extern getbootdrive
extern getchar
extern set_input_mode
extern vga_set_color
extern load_elf
extern getfileinfo
extern getfilelist
extern get_time_ms
extern vga_write_pixel
extern random
extern vga_draw_rect
extern sleep
extern panic
extern vga_set_mode
extern vga_init_objects
extern vga_deinit_objects
extern vga_add_object
extern vga_remove_object
extern vga_render_objects
_start:

ret
default_master_isr:
cli
pusha
mov al, 0x20
mov dx, 0x20
out dx, al
popa
sti
iret
default_slave_isr:
cli
pusha
mov al, 0x20
mov dx, 0xA0
out dx, al
popa
sti
iret
timer_isr:
cli
pusha
call timer_interrupt
mov al, 0x20
mov dx, 0x20
out dx, al
popa
sti
iret
keyboard_isr:
cli
pusha
call keyboard_interrupt
mov al, 0x20
mov dx, 0x20
out dx, al
popa
sti
iret
mouse_isr:
cli
pusha
call mouse_interrupt
mov al, 0x20
mov dx, 0x20
out dx, al
popa
sti
iret
isr0:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 0
push eax 
call exception_handler
iret
isr1:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 1
push eax 
call exception_handler
iret
isr2:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 2
push eax 
call exception_handler
iret
isr3:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 3
push eax 
call exception_handler
iret
isr4:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 4
push eax 
call exception_handler
iret
isr5:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 5
push eax 
call exception_handler
iret
isr6:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 6
push eax 
call exception_handler
iret
isr7:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 7
push eax 
call exception_handler
iret
isr8:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 8
push eax 
call exception_handler
iret
isr9:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 9
push eax 
call exception_handler
iret
isr10:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 10
push eax 
call exception_handler
iret
isr11:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 11
push eax 
call exception_handler
iret
isr12:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 12
push eax 
call exception_handler
iret
isr13:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 13
push eax 
call exception_handler
iret
isr14:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 14
push eax 
call exception_handler
iret
isr16:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 16
push eax 
call exception_handler
iret
isr17:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 17
push eax 
call exception_handler
iret
isr18:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 18
push eax 
call exception_handler
iret
isr19:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 19
push eax 
call exception_handler
iret
isr20:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 20
push eax 
call exception_handler
iret
isr21:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 21
push eax 
call exception_handler
iret
isr28:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 28
push eax 
call exception_handler
iret
isr29:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 29
push eax 
call exception_handler
iret
isr30:
cli
push esp
push ebp
push edx
push ecx
push ebx
push eax
mov dword eax, 30
push eax 
call exception_handler
iret
syscall_isr:
sti
add esp, 12
xor eax, eax
cmp edx, 0
je putchar
cmp edx, 1
je print
cmp edx, 2
je puthex
cmp edx, 3
je kmalloc
cmp edx, 4
je kfree
cmp edx, 5
je openfile
cmp edx, 6
je closefile
cmp edx, 7
je deletefile
cmp edx, 8
je getfilesize
cmp edx, 9
je writefile
cmp edx, 10
je readfile
cmp edx, 11
je getbootdrive
cmp edx, 12
je clear
cmp edx, 13
je getchar
cmp edx, 14
je set_input_mode
cmp edx, 15
je vga_set_color
cmp edx, 16
je load_elf
cmp edx, 17
je getfileinfo
cmp edx, 18
je createfile
cmp edx, 19
je getfilelist
cmp edx, 20
je renamefile
cmp edx, 21
je get_time_ms
cmp edx, 22
je vga_write_pixel
cmp edx, 23
je random
cmp edx, 24
je vga_draw_rect
cmp edx, 25
je sleep
cmp edx, 26
je panic
cmp edx, 27
je vga_init_objects
cmp edx, 28
je vga_deinit_objects
cmp edx, 29
je vga_add_object
cmp edx, 30
je vga_remove_object
cmp edx, 31
je vga_render_objects
syscall_end:
ret
ebxmsg db "edx %d", 0
msg db "calling render objects %d", 0
