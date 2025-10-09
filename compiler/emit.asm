global bytecode_emit
global bytecode_init
global bytecode_deinit
extern sys_kmalloc
extern sys_kfree
msg db "bytecode: %p", 10, 0
bytecode_emit:
push ebp
mov dword ebp, esp
mov dword eax, [esp+4]
mov dword ebx, [esp+8]
xor eax, eax
leave
ret
bytecode_init:
push ebp
mov dword ebp, esp
sub dword esp, 4
mov dword [esp], 8
call sys_kmalloc
add dword esp, 4
cmp eax, 0
je bytecode_init_fail 
mov dword ebx, eax
mov dword ecx, [esp+8]
push ebx
push ecx
call sys_kmalloc
pop ecx
pop ebx
mov dword [ebx], eax
mov dword [ebx+4], ecx
bytecode_init_end:
leave
mov dword eax, ebx
ret
bytecode_init_fail:
leave
xor eax, eax
ret
bytecode_deinit:
push ebp
mov dword ebp, esp
leave
xor eax, eax
ret
