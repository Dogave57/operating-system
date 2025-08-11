CFLAGS='-nostdlib -ffreestanding'
LDFLAGS=''
nasm -f bin bootloader.asm -o bootloader.bin
nasm -f elf32 isrs.asm -o isrs.o
i686-elf-gcc -c -O0 kernel.c $CFLAGS -T linker.ld -o kernel.o
i686-elf-gcc -c -O0 commands.c $CFLAGS -o commands.o
i686-elf-gcc -c -O0 video.c $CFLAGS -o video.o
i686-elf-gcc -c -O0 stdlib.c $CFLAGS -o stdlib.o
i686-elf-gcc -c -O0 idt.c $CFLAGS -o idt.o
i686-elf-gcc -c -O0 cursor.c $CFLAGS -o cursor.o
i686-elf-gcc -c -O0 panic.c $CFLAGS -o panic.o
i686-elf-gcc -c -O0 memory.c $CFLAGS -o memory.o
i686-elf-gcc -c -O0 filesystem.c $CFLAGS -o filesystem.o
i686-elf-ld -T linker.ld kernel.o video.o stdlib.o isrs.o idt.o commands.o cursor.o panic.o memory.o filesystem.o -o kernel.elf
i686-elf-objcopy -O binary kernel.elf kernel.bin
truncate -s 8192 kernel.bin
cat bootloader.bin kernel.bin > os.img
