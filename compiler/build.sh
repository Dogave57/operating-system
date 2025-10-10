CC='i686-elf-gcc'
AS='nasm'
LD='i686-elf-ld'
CFLAGS='-nostdlib -ffreestanding'
$CC -c main.c $CFLAGS -o main.o
$CC -c emit.c $CFLAGS -o emit.o
$LD -pie -nostdlib main.o emit.o '../stdlib.o' '../libsys.o' -o compiler.elf

