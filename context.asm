global context_switch
context_switch:
;mov dword eax, [esp+16]
;mov dword ebx, [esp+20]
;mov dword ecx, [esp+24]
;mov dword edx, [esp+28]
;mov dword ebp, [esp+12]
;mov dword esp, [esp+8]
jmp [esp+4]
ret
