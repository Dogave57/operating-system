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
mov al, 8
xor ch, ch
mov cl, 2
xor dh, dh
mov dl, bl
mov bx, 0x7E30
int 0x13
jc readerr_handler
;null desc
mov word [0x7E00], 0xFFFF
mov word [0x7E00+2], 0x0
mov byte [0x7E00+4], 0x0
mov byte [0x7E00+5], 0x0
mov byte [0x7E00+6], 0x0
mov byte [0x7E00+7], 0x0
;code desc
mov word [0x7E00+8], 0xFFFF;limit: 2 bytes
mov word [0x7E00+10], 0x0;low base: 2 bytes
mov byte [0x7E00+12], 0x0;middle base: 1 byte
mov byte [0x7E00+13], 0x9A;access: 1 byte
mov byte [0x7E00+14], 0xCF;flags: 1 byte
mov byte [0x7E00+15], 0x0;base high: 1 byte
;data desc
mov word [0x7E00+16], 0xFFFF
mov word [0x7E00+18], 0x0
mov byte [0x7E00+20], 0x0
mov byte [0x7E00+21], 0x92
mov byte [0x7E00+22], 0xCF
mov byte [0x7E00+23], 0x0
;gdtr
mov word [0x7E00+24], 23
mov word [0x7E00+26], 0x7E00
mov word [0x7E00+28], 0x0
lgdt [0x7E00+24]
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
mov esp, 20000
jmp 0x08:0x7E30
hlt
ret
readerr_handler:
mov si, readerrmsg
call print
hlt
ret
hlt
ret
BITS 32
protected:
mov byte [0xB8000], 'Z'
mov byte [0xB8001], 0x07
jmp 0x08:0x7E30
hlt
ret
BITS 16
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
bootmsg db "booting extended x86 protected bootloader", 0
readerrmsg db "failed to read protected bootloader", 0
times 510-($-$$) db 0
dw 0xAA55

