CFLAGS='-nostdlib -ffreestanding'
LDFLAGS=''
nasm -f bin bootloader.asm -o bootloader.bin
nasm -f elf32 isrs.asm -o isrs.o
nasm -f elf32 libsys.asm -o libsys.o
i686-elf-gcc -c -O0 kernel.c $CFLAGS -T linker.ld -o kernel.o
i686-elf-gcc -c -O0 commands.c $CFLAGS -o commands.o
i686-elf-gcc -c -O0 video.c $CFLAGS -o video.o
i686-elf-gcc -c -O0 stdlib.c $CFLAGS -o stdlib.o
i686-elf-gcc -c -O0 idt.c $CFLAGS -o idt.o
i686-elf-gcc -c -O0 cursor.c $CFLAGS -o cursor.o
i686-elf-gcc -c -O0 panic.c $CFLAGS -o panic.o
i686-elf-gcc -c -O0 memory.c $CFLAGS -o memory.o
i686-elf-gcc -c -O0 filesystem.c $CFLAGS -o filesystem.o
i686-elf-gcc -c -O0 timer.c $CFLAGS -o timer.o
i686-elf-gcc -c -O0 thread.c $CFLAGS -o thread.o
i686-elf-gcc -c -O0 exception.c $CFLAGS -o exception.o
i686-elf-gcc -c -O0 usb.c $CFLAGS -o usb.o
i686-elf-gcc -c -O0 pci.c $CFLAGS -o pci.o
i686-elf-gcc -c -O0 smbios.c $CFLAGS -o smbios.o
i686-elf-gcc -c -O0 dev.c $CFLAGS -o dev.o
i686-elf-gcc -c -O0 loader.c $CFLAGS -o loader.o
i686-elf-gcc -c -O0 shell.c $CFLAGS -o shell.o
i686-elf-gcc -c -O0 calc.c $CFLAGS -o calc.o
echo linking kernel
i686-elf-ld -T linker.ld kernel.o video.o stdlib.o isrs.o idt.o commands.o cursor.o panic.o memory.o filesystem.o timer.o thread.o exception.o usb.o pci.o smbios.o dev.o loader.o libsys.o -o kernel.elf
i686-elf-objcopy -O binary kernel.elf kernel.bin
i686-elf-ld -pie shell.o libsys.o stdlib.o -o shell.elf
i686-elf-ld -pie calc.o libsys.o stdlib.o -o calc.elf
truncate -s 65536 kernel.bin
truncate -s 0 os.img
cat bootloader.bin kernel.bin > os.img
sudo cat os.img > drive.img
sudo gcc -O0 fsgen/main.c -o fsgen/main
cp shell.elf fsgen/files/programs/shell.elf
cp calc.elf fsgen/files/programs/calc.elf
echo generating filesystem
sudo ./'fsgen/main' fsgen/files drive.img 8000000
echo done generating filesystem
