global default_master_isr
global default_slave_isr
global timer_isr
global keyboard_isr
extern timer_interrupt
extern keyboard_interrupt
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
call timer_interrupt
mov al, 0x20
mov dx, 0x20
out dx, al
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
