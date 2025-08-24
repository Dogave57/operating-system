BITS 16
org 0x7C00
section .text
_start:
cli
mov bl, dl
mov [0x2000], dl
mov si, bootmsg
call print
clc
mov ah, 0x2
mov al, 64
xor ch, ch
mov cl, 2
xor dh, dh
mov dl, bl
mov bx, 0x9030
int 0x13
jc readerr_handler
;null desc
mov word [0x9000], 0xFFFF
mov word [0x9000+2], 0x0
mov byte [0x9000+4], 0x0
mov byte [0x9000+5], 0x0
mov byte [0x9000+6], 0x0
mov byte [0x9000+7], 0x0
;code desc
mov word [0x9000+8], 0xFFFF;limit: 2 bytes
mov word [0x9000+10], 0x0;low base: 2 bytes
mov byte [0x9000+12], 0x0;middle base: 1 byte
mov byte [0x9000+13], 0x9A;access: 1 byte
mov byte [0x9000+14], 0xCF;flags: 1 byte
mov byte [0x9000+15], 0x0;base high: 1 byte
;data desc
mov word [0x9000+16], 0xFFFF
mov word [0x9000+18], 0x0
mov byte [0x9000+20], 0x0
mov byte [0x9000+21], 0x92
mov byte [0x9000+22], 0xCF
mov byte [0x9000+23], 0x0
;gdtr
mov word [0x9000+24], 23
mov word [0x9000+26], 0x9000
mov word [0x9000+28], 0x0
lgdt [0x9000+24]
xor eax, eax
cpuid
mov dword [0x2000+4], ebx
mov dword [0x2000+8], edx
mov dword [0x2000+12], ecx
mov byte [0x2000+16], 0
mov eax, 0x80000002
cpuid
mov dword [0x2000+17], eax
mov dword [0x2000+21], ebx
mov dword [0x2000+25], ecx
mov dword [0x2000+29], edx
mov eax, 0x80000003
cpuid
mov dword [0x2000+33], eax
mov dword [0x2000+37], ebx
mov dword [0x2000+41], ecx
mov dword [0x2000+45], edx
mov eax, 0x80000004
cpuid
mov dword [0x2000+49], eax
mov dword [0x2000+53], ebx
mov dword [0x2000+57], ecx
mov dword [0x2000+61], edx
mov byte [0x2000+65], 0
mov dword [0x2000+68], 0x8004
mov dword [0x2000+72], 0
xor ax, ax
mov es, ax
mov di, 0x8004
memorymaploop:
mov eax, 0xE820
mov edx, 0x534D4150
mov ecx, 24
int 0x15
jc bootfail_handler
cmp eax, 0x534D4150
jne bootfail_handler
add dword [0x2000+72], 1
cmp ebx, 0
je memorymaploopend
add di, 24
jmp memorymaploop
memorymaploopend:
mov es, ax
xor ah, ah
mov al, 0x03
int 0x10
mov eax, cr0
or eax, 1
mov cr0, eax
mov ax, 0x10
mov ds, ax
mov gs, ax
mov ss, ax
mov es, ax
mov fs, ax
mov esp, 200000
jmp 0x08:0x9030
hlt
ret
readerr_handler:
mov si, readerrmsg
call print
hlt
ret
bootfail_handler:
mov si, bootfail
call print
hlt
ret
hlt
ret
hlt
ret
print:
mov ah, 0x0E
xor al, al
printloop:
lodsb
cmp al, 0
je printloopend
int 0x10
jmp printloop
printloopend:
mov ah, 0x0E
mov al, 13
int 0x10
mov al, 10
int 0x10
ret
bootmsg db "booting...", 0
readerrmsg db "failed to read kernel", 0
bootfail db "failed to boot", 0
times 510-($-$$) db 0
dw 0xAA55

