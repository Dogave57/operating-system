CFLAGS='-nostdlib -ffreestanding'
LDFLAGS=''
nasm -f bin bootloader.asm -o bootloader.bin
nasm -f elf32 isrs.asm -o isrs.o
i686-elf-gcc -c -O0 -fno-pic kernel.c $CFLAGS -T linker.ld -o kernel.o
i686-elf-gcc -c -O0 -fno-pic commands.c $CFLAGS -o commands.o
i686-elf-gcc -c -O0 -fno-pic video.c $CFLAGS -o video.o
i686-elf-gcc -c -O0 -fno-pic stdlib.c $CFLAGS -o stdlib.o
i686-elf-gcc -c -O0 -fno-pic idt.c $CFLAGS -o idt.o
i686-elf-gcc -c -O0 -fno-pic cursor.c $CFLAGS -o cursor.o
i686-elf-gcc -c -O0 -fno-pic panic.c $CFLAGS -o panic.o
i686-elf-gcc -c -O0 -fno-pic memory.c $CFLAGS -o memory.o
i686-elf-gcc -c -O0 -fno-pic filesystem.c $CFLAGS -o filesystem.o
i686-elf-gcc -c -O0 -fno-pic timer.c $CFLAGS -o timer.o
i686-elf-gcc -c -O0 -fno-pic thread.c $CFLAGS -o thread.o
i686-elf-gcc -c -O0 -fno-pic exception.c $CFLAGS -o exception.o
i686-elf-gcc -c -O0 -fno-pic usb.c $CFLAGS -o usb.o
i686-elf-gcc -c -O0 -fno-pic pci.c $CFLAGS -o pci.o
i686-elf-ld -T linker.ld kernel.o video.o stdlib.o isrs.o idt.o commands.o cursor.o panic.o memory.o filesystem.o timer.o thread.o exception.o usb.o pci.o -o kernel.elf
i686-elf-objcopy -O binary kernel.elf kernel.bin
truncate -s 16384 kernel.bin
truncate -s 0 os.img
cat bootloader.bin kernel.bin > os.img
