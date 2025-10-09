global proc_bootstrap
extern printf
extern print
extern putchar
extern switch_task
callmsg db "calling entry",10, 0
finishmsg db "program finished execution", 10, 0
stackmsg db "before switch: %p", 10, "after switch: %p", 10, 0
test:
xor edx, edx
sub esp, 4
mov dword [esp], 's'
call putchar
add esp, 4
ret
proc_bootstrap:
push ebp
mov ebp, esp
mov eax, [esp+8]
mov ebx, [eax+8]
mov ecx, [eax+12]
mov edx, [eax]
mov dword ebx, [esp]
sub esp, 8
mov dword [esp], ebx
mov dword [esp+4], ecx
jmp edx
add esp, 8
sub esp, 4
mov dword [esp], finishmsg
call print
add esp, 4
b:
jmp b
leave
ret
