global _start
global msg
_start:
mov ebx, msg
int 90h
ret
msg db "shell loaded", 0
