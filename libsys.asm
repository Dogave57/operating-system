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
sys_putchar:
xor edx, edx
mov ebx, [esp+4]
int 90h
ret
sys_print:
mov edx, 1
mov ebx, [esp+4]
int 90h
ret
sys_puthex:
mov edx, 2
mov ebx, [esp+4]
mov ecx, [esp+8]
int 90h
ret
sys_kmalloc:
mov edx, 3
mov ebx, [esp+4]
int 90h
ret
sys_kfree:
mov edx, 4
mov ebx, [esp+4]
int 90h
ret
sys_openfile:
mov edx, 5
mov ebx, [esp+4]
mov ecx, [esp+8]
int 90h
ret
sys_closefile:
mov edx, 6
mov ebx, [esp+4]
int 90h
ret
sys_deletefile:
mov edx, 7
mov ebx, [esp+4]
int 90h
ret
sys_getfilesize:
mov edx, 8
mov ebx, [esp+4]
int 90h
ret
sys_writefile:
mov edx, 9
mov ebx, [esp+4]
mov ecx, [esp+8]
;mov edx, [esp+12]
int 90h
ret
sys_readfile:
mov edx, 10
mov ebx, [esp+4]
mov ecx, [esp+8]
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
