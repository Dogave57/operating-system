CFLAGS='-nostdlib -ffreestanding'
LDFLAGS=''
CC='i686-elf-gcc'
LD='i686-elf-ld'
OBJCPY='i686-elf-objcopy'
AS='nasm'
$AS -f bin bootloader.asm -o bootloader.bin
$AS -f elf32 isrs.asm -o isrs.o
$AS -f elf32 libsys.asm -o libsys.o
$AS -f elf32 context.asm -o context.o
$AS -f elf32 loader.asm -o loader_asm.o
$CC -c -O0 kernel.c $CFLAGS -T linker.ld -o kernel.o
$CC -c -O0 commands.c $CFLAGS -o commands.o
$CC -c -O0 video.c $CFLAGS -o video.o
$CC -c -O0 stdlib.c $CFLAGS -o stdlib.o
$CC -c -O0 idt.c $CFLAGS -o idt.o
$CC -c -O0 cursor.c $CFLAGS -o cursor.o
$CC -c -O0 panic.c $CFLAGS -o panic.o
$CC -c -O0 memory.c $CFLAGS -o memory.o
$CC -c -O0 filesystem.c $CFLAGS -o filesystem.o
$CC -c -O0 timer.c $CFLAGS -o timer.o
$CC -c -O0 thread.c $CFLAGS -o thread.o
$CC -c -O0 exception.c $CFLAGS -o exception.o
$CC -c -O0 usb.c $CFLAGS -o usb.o
$CC -c -O0 pci.c $CFLAGS -o pci.o
$CC -c -O0 smbios.c $CFLAGS -o smbios.o
$CC -c -O0 dev.c $CFLAGS -o dev.o
$CC -c -O0 loader.c $CFLAGS -o loader.o
$CC -c -O0 speaker.c $CFLAGS -o speaker.o
$CC -c -O0 shell.c $CFLAGS -o shell.o
$CC -c -O0 flappybird.c $CFLAGS -o flappybird.o
$CC -c -O0 musicedit.c $CFLAGS -o musicedit.o
$CC -c -O0 textedit.c $CFLAGS -o textedit.o
$CC -c -O0 imgview.c $CFLAGS -o imgview.o
$CC -c -O0 math.c $CFLAGS -o math.o
cd compiler
bash build.sh
cd ..
echo linking kernel
$LD -T linker.ld kernel.o video.o stdlib.o isrs.o idt.o commands.o cursor.o panic.o memory.o filesystem.o timer.o thread.o exception.o usb.o pci.o smbios.o dev.o loader.o libsys.o speaker.o context.o math.o loader_asm.o -o kernel.elf
$OBJCPY -O binary kernel.elf kernel.bin
$LD -pie -nostdlib shell.o libsys.o stdlib.o math.o -o shell.elf
$LD -pie -nostdlib flappybird.o libsys.o stdlib.o -o flappybird.elf
$LD -pie -nostdlib musicedit.o libsys.o stdlib.o -o musicedit.elf
$LD -pie -nostdlib textedit.o libsys.o stdlib.o -o textedit.elf
$LD -pie -nostdlib imgview.o libsys.o stdlib.o -o imgview.elf
$LD -pie -nostdlib compiler.o libsys.o stdlib.o -o compiler.elf
truncate -s 65536 kernel.bin
truncate -s 0 os.img
cat bootloader.bin kernel.bin > os.img
sudo cat os.img > drive.img
sudo gcc -O0 fsgen/main.c -o fsgen/main
cp shell.elf fsgen/files/programs/shell.elf
cp flappybird.elf fsgen/files/programs/flappybird.elf
cp musicedit.elf fsgen/files/programs/musicedit.elf
cp textedit.elf fsgen/files/programs/textedit.elf
cp imgview.elf fsgen/files/programs/imgview.elf
cp compiler/compiler.elf fsgen/files/programs/compiler.elf
echo generating filesystem
sudo ./'fsgen/main' fsgen/files drive.img 1000000
echo done generating filesystem
