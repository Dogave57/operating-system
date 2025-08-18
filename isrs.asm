global default_master_isr
global default_slave_isr
global timer_isr
global keyboard_isr
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
extern timer_interrupt
extern keyboard_interrupt
extern printf
extern exception_handler
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
call keyboard_interrupt
pusha
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
msg db "eip: %p", 10
db 0
