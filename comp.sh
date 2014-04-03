i586-elf-as boot.s -o boot.o &&
i586-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra &&
i586-elf-gcc -T linker.ld -o zmey.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc &&
qemu-system-i386 -curses -kernel zmey.bin

