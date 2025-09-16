global sys_getsyscall_list
sys_getsyscall_list:
mov dword eax, [esp+4]
int 90h
ret
