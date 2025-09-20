global print
global putchar
print:
mov ebx, [esp+4]
mov eax, 1
int 90h
ret
putchar:
mov ebx, [esp+4]
xor eax, eax
int 90h
ret
