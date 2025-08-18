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
i686-elf-ld -T linker.ld kernel.o video.o stdlib.o isrs.o idt.o commands.o cursor.o panic.o memory.o filesystem.o timer.o thread.o exception.o -o kernel.elf
i686-elf-objcopy -O binary kernel.elf kernel.bin
truncate -s 16384 kernel.bin
truncate -s 0 os.img
cat bootloader.bin kernel.bin > os.img
#sudo dd if=/dev/zero of=drive.img bs=1M count=16
#sudo parted --script drive.img mklabel msdos
#sudo parted --script drive.img mkpart primary fat32 64KiB 100%
#sudo dd if=bootloader.bin of=drive.img bs=446 count=1 conv=notrunc
#sudo dd if=kernel.bin of=drive.img bs=512 seek=1 count=128 conv=notrunc
#LOOP=$(sudo losetup -fP --show drive.img)
#echo ${LOOP}
#sudo mkfs.fat -F 32 ${LOOP}p1
#sudo mount ${LOOP}p1 disk_mnt
#sudo cp -r osfiles/* disk_mnt
#sudo umount disk_mnt
#sudo losetup -d ${LOOP}
