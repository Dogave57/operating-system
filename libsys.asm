global sys_putchar
global sys_print
global sys_puthex
global sys_kmalloc
global sys_kfree
global sys_openfile
global sys_closefile
global sys_deletefile
global sys_getfilesize
global sys_writefile
global sys_readfile
global sys_getbootdrive
global sys_clear
global sys_getchar
global sys_set_console_color
global sys_loadelf
global sys_getfileinfo
global sys_createfile
global sys_getfilelist
global sys_renamefile
global sys_get_time_ms
global sys_writepixel
global sys_random
global sys_draw_rect
global sys_sleep
global sys_panic
global sys_init_objects
global sys_deinit_objects
global sys_add_rect
global sys_remove_object
global sys_render_objects
global sys_keypressed
global sys_set_bg
global sys_add_text
global sys_readpixel
global sys_add_vertices
global sys_playsound
global sys_stopsound
global sys_beep
global sys_reboot
sys_putchar:
xor edx, edx
int 90h
ret
sys_print:
mov edx, 1
int 90h
ret
sys_puthex:
mov edx, 2
int 90h
retn 
sys_kmalloc:
mov edx, 3
int 90h
ret
sys_kfree:
mov edx, 4
int 90h
ret
sys_openfile:
mov edx, 5
int 90h
ret
sys_closefile:
mov edx, 6
int 90h
ret
sys_deletefile:
mov edx, 7
int 90h
ret
sys_getfilesize:
mov edx, 8
int 90h
ret
sys_writefile:
mov edx, 9
int 90h
ret
sys_readfile:
mov edx, 10
int 90h
ret
sys_getbootdrive:
mov edx, 11
int 90h
ret
sys_clear:
mov edx, 12
int 90h
ret
sys_getchar:
mov edx, 13
int 90h
ret
sys_set_console_color:
mov edx, 14
int 90h
ret
sys_loadelf:
mov edx, 15
int 90h
ret
sys_getfileinfo:
mov edx, 16
int 90h
ret
sys_createfile:
mov edx, 17
int 90h
ret
sys_getfilelist:
mov edx, 18
int 90h
ret
sys_renamefile:
mov edx, 19
int 90h
ret
sys_get_time_ms:
mov edx, 20
int 90h
ret
sys_writepixel:
mov edx, 21
int 90h
ret
sys_random:
mov edx, 22
int 90h
ret
sys_draw_rect:
mov edx, 23
int 90h
ret
sys_sleep:
mov edx, 24
int 90h
ret
sys_panic:
mov edx, 25
int 90h
ret
sys_init_objects:
mov edx, 26
int 90h
ret
sys_deinit_objects:
mov edx, 27
int 90h
ret
sys_add_rect:
mov edx, 28
int 90h
ret
sys_remove_object:
mov edx, 29
int 90h
ret
sys_render_objects:
mov edx, 30
int 90h
ret
sys_keypressed:
mov edx, 31
int 90h
ret
sys_set_bg:
mov edx, 32
int 90h
ret
sys_add_text:
mov edx, 33
int 90h
ret
sys_readpixel:
mov edx, 34
int 90h
ret
sys_add_vertices:
mov edx, 35
int 90h
ret
sys_playsound:
mov edx, 36
int 90h
ret
sys_stopsound:
mov edx, 37
int 90h
ret
sys_beep:
mov edx, 38
int 90h
ret
sys_reboot:
mov edx, 39
int 90h
ret
