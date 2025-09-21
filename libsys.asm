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
global sys_set_input_mode
global sys_set_console_color
global sys_loadelf
global sys_getfileinfo
global sys_createfile
global sys_getfilelist
global sys_renamefile
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
ret
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
sys_set_input_mode:
mov edx, 14
int 90h
ret
sys_set_console_color:
mov edx, 15
int 90h
ret
sys_loadelf:
mov edx, 16
int 90h
ret
sys_getfileinfo:
mov edx, 17
int 90h
ret
sys_createfile:
mov edx, 18
int 90h
ret
sys_getfilelist:
mov edx, 19
int 90h
ret
sys_renamefile:
mov edx, 20
int 90h
ret
