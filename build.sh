CFLAGS='-nostdlib -ffreestanding'
LDFLAGS=''
nasm -f bin bootloader.asm -o bootloader.bin
nasm -f elf32 isrs.asm -o isrs.o
i686-elf-gcc -c kernel.c $CFLAGS -T linker.ld -o kernel.o
i686-elf-gcc -c commands.c $CFLAGS -o commands.o
i686-elf-gcc -c video.c $CFLAGS -o video.o
i686-elf-gcc -c stdlib.c $CFLAGS -o stdlib.o
i686-elf-gcc -c idt.c $CFLAGS -o idt.o
i686-elf-gcc -c cursor.c $CFLAGS -o cursor.o
i686-elf-gcc -c panic.c $CFLAGS -o panic.o
i686-elf-ld -T linker.ld kernel.o video.o stdlib.o isrs.o idt.o commands.o cursor.o panic.o -o kernel.elf
i686-elf-objcopy -O binary kernel.elf kernel.bin
truncate -s 4096 kernel.bin
cat bootloader.bin kernel.bin > os.img
