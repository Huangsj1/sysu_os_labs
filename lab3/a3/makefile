build:
	nasm -f bin mbr.asm -o mbr.bin
	#nasm -f elf32 bootloader.asm -o bootloader.bin
	#nasm -f elf32 bullet.asm -o bullet.bin
	nasm -f elf32 bootloader.asm -o bootloader.o
	nasm -f elf32 bullet.asm -o bullet.o
	ld -o bootloader.bin -melf_i386 -N bootloader.o -Ttext 0x7e00 --oformat binary
	ld -o bullet.bin -melf_i386 -N bullet.o -Ttext 0xa000 --oformat binary
	dd if=mbr.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader.bin of=hd.img bs=512 count=5 seek=1 conv=notrunc
	dd if=bullet.bin of=hd.img bs=512 count=1 seek=6 conv=notrunc
run:
	qemu-system-i386 -hda hd.img -serial null -parallel stdio
clean:
	rm -rf *.bin
debug:
	qemu-system-i386 -s -S -hda hd.img -serial null -parallel stdio &
	sleep 1
	gnome-terminal -e "gdb -q -x gdbinit"
build_gdb:
	nasm -g -f elf32 mbr.asm -o mbr.o
	ld -o mbr.symbol -melf_i386 -N mbr.o -Ttext 0x7c00
	ld -o mbr.bin -melf_i386 -N mbr.o -Ttext 0x7c00 --oformat binary
	nasm -g -f elf32 bootloader.asm -o bootloader.o
	ld -o bootloader.symbol -melf_i386 -N bootloader.o -Ttext 0x7e00
	ld -o bootloader.bin -melf_i386 -N bootloader.o -Ttext 0x7e00 --oformat binary
	nasm -g -f elf32 bullet.asm -o bullet.o
	ld -o bullet.symbol -melf_i386 -N bullet.o -Ttext 0xa000
	ld -o bullet.bin -melf_i386 -N bullet.o -Ttext 0xa000 --oformat binary
	dd if=mbr.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader.bin of=hd.img bs=512 count=5 seek=1 conv=notrunc
	dd if=bullet.bin of=hd.img bs=512 count=1 seek=6 conv=notrunc
clean_gdb:
	rm -rf *.o *.bin *.symbol
