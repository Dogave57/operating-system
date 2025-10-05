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
extern vga_add_rect
extern vga_remove_object
extern vga_render_objects
extern key_pressed
extern vga_set_bg
extern vga_add_text
extern vga_read_pixel
extern vga_add_vertices
extern play_sound
extern stop_sound
extern beep
extern reboot
extern time_ms
extern scheduler_info
extern first_thread
extern vga_write_char
extern vga_draw_rect
extern vga_write_coord
extern vga_set_char_position
extern switch_task
switchmsg db "ctx switch", 10, 0
tpmsg db "new thread: %p", 10, 0
eipmsg db "eip: %p", 10, 0
tidmsg db "tid: %d", 10, 0
teipmsg db "thread eip: %p", 10, 0
schedulerfail db "scheduler failure!", 10, 0
scheduler_init:
mov dword [scheduler_info+4], 0
ret
switch_task:
pushad
mov dword eax, [scheduler_info+4]
cmp eax, 0
je change_task
mov dword ebx, [esp+32]
mov dword [eax+12], ebx
mov dword ebx, [esp]
mov dword [eax+44], ebx;edi
mov dword ebx, [esp+4]
mov dword [eax+40], ebx;esi
mov dword ebx, [esp+8]
mov dword [eax+20], ebx;ebp
mov dword ebx, [esp+12]
mov dword [eax+16], ebx;esp
mov dword ebx, [esp+16]
mov dword [eax+28], ebx;ebx
mov dword ebx, [esp+20]
mov dword [eax+36], ebx;edx
mov dword ebx, [esp+24]
mov dword [eax+32], ebx;ecx
mov dword ebx, [esp+28]
mov dword [eax+24], ebx;eax
xor eax, eax
change_task:
popad
mov dword ebx, [esp+4]
cmp ebx, 0
je random_task
mov dword eax, ebx
jmp switch
random_task:
mov dword eax, [scheduler_info+4]
cmp eax, 0
jne new_task
first_task:
mov dword eax, [first_thread]
jmp switch
new_task:
mov dword eax, [eax+4]
jmp switch
switch:
cmp eax, 0
jne success
mov eax, -1
ret
success:
mov dword [scheduler_info+4], eax
sub esp, 4
mov dword [esp], eax
jmp ctx_switch
xor eax, eax
ret
ctx_switch:
mov al, 20h
mov dx, 20h
out dx, al
mov dword eax, [esp]
add esp, 4
mov dword esp, [eax+16]
sub esp, 32
mov dword ebx, [eax+12]
mov dword [esp], ebx;eip
mov dword ebx, [eax+20]
mov dword [esp+4], ebx;ebp
mov dword ebx, [eax+24]
mov dword [esp+8], ebx;eax
mov dword ebx, [eax+28]
mov dword [esp+12], ebx;ebx
mov dword ebx, [eax+32]
mov dword [esp+16], ebx;ecx
mov dword ebx, [eax+36]
mov dword [esp+20], ebx;edx
mov dword ebx, [eax+40]
mov dword [esp+24], ebx;esi
mov dword ebx, [eax+44]
mov dword [esp+28], ebx;edi
mov dword ebp, [esp+4]
mov dword eax, [esp+8]
mov dword ebx, [esp+12]
mov dword ecx, [esp+16]
mov dword edx, [esp+20]
sti
add esp, 32
jmp [esp-32]
ret
timer_interrupt:
cli
pushad
mov dword eax, [scheduler_info+4]
cmp eax, 0
jne timer_change
call scheduler_init
timer_change:
add dword [time_ms], 1
end:
popa
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
pushad
call timer_interrupt
mov al, 0x20
mov dx, 0x20
out dx, al
popad
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
pushad
call mouse_interrupt
mov al, 0x20
mov dx, 0x20
out dx, al
popad
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
je vga_set_color
cmp edx, 15
je load_elf
cmp edx, 16
je getfileinfo
cmp edx, 17
je createfile
cmp edx, 18
je getfilelist
cmp edx, 19
je renamefile
cmp edx, 20
je get_time_ms
cmp edx, 21
je vga_write_pixel
cmp edx, 22
je random
cmp edx, 23
je vga_draw_rect
cmp edx, 24
je sleep
cmp edx, 25
je panic
cmp edx, 26
je vga_init_objects
cmp edx, 27
je vga_deinit_objects
cmp edx, 28
je vga_add_rect
cmp edx, 29
je vga_remove_object
cmp edx, 30
je vga_render_objects
cmp edx, 31
je key_pressed
cmp edx, 32
je vga_set_bg
cmp edx, 33
je vga_add_text
cmp edx, 34
je vga_read_pixel
cmp edx, 35
je vga_add_vertices
cmp edx, 36
je play_sound
cmp edx, 37
je stop_sound
cmp edx, 38
je beep
cmp edx, 39
je reboot
cmp edx, 40
je vga_write_char
cmp edx, 41
je vga_draw_rect
cmp edx, 42
je vga_write_coord
cmp edx, 43
je vga_set_char_position
cmp edx, 44
je switch_task
syscall_end:
ret
ebxmsg db "edx %d", 0
msg db "calling render objects %d", 0
