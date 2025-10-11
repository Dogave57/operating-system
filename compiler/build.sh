CC='i686-elf-gcc'
AS='nasm'
LD='i686-elf-ld'
CFLAGS='-nostdlib -ffreestanding'
$CC -O0 -c main.c $CFLAGS -o main.o
$CC -O0 -c emit.c $CFLAGS -o emit.o
$LD -pie -nostdlib main.o emit.o '../stdlib.o' '../libsys.o' -o compiler.elf

