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
sys_putchar:
mov ebx, [esp+4]
xor eax, eax
int 90h
ret
sys_print:
mov ebx, [esp+4]
mov eax, 1
int 90h
ret
sys_puthex:
mov ebx, [esp+8]
mov ecx, [esp+4]
mov eax, 2
int 90h
ret
sys_kmalloc:
mov ebx, [esp+4]
mov eax, 3
int 90h
ret
sys_kfree:
mov ebx, [esp+4]
mov eax, 4
int 90h
ret
sys_openfile:
mov ebx, [esp+4]
mov ecx, [esp+8]
mov eax, 5
int 90h
ret
sys_closefile:
mov ebx, [esp+4]
mov eax, 6
int 90h
ret
sys_deletefile:
mov ebx, [esp+4]
mov eax, 7
int 90h
ret
sys_getfilesize:
mov ebx, [esp+4]
mov eax, 8
int 90h
ret
sys_writefile:
mov ebx, [esp+4]
mov ecx, [esp+8]
mov edx, [esp+12]
mov eax, 9
int 90h
ret
sys_readfile:
mov ebx, [esp+4]
mov ecx, [esp+8]
mov eax, 10
int 90h
ret
sys_getbootdrive:
mov eax, 11
int 90h
ret
