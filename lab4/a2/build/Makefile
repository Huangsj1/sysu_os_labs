ASM_COMPILER = nasm
C_COMPLILER = gcc
CPP_COMPILER = g++
CPP_COMPILER_FLAGS = -g -Wall -march=i386 -m32 -nostdlib -fno-builtin -ffreestanding -fno-pic
LINKER = ld

SRCDIR = ../src
RUNDIR = ../run
BUILDDIR = build
INCLUDE_PATH = ../include

CPP_SOURCE += $(wildcard $(SRCDIR)/kernel/*.cpp)
CPP_OBJ += $(CPP_SOURCE:$(SRCDIR)/kernel/%.cpp=%.o)

ASM_SOURCE += $(wildcard $(SRCDIR)/utils/*.asm)
ASM_OBJ += $(ASM_SOURCE:$(SRCDIR)/utils/%.asm=%.o)

OBJ += $(CPP_OBJ)
OBJ += $(ASM_OBJ)

build: mbr.bin bootloader.bin kernel.bin kernel.o
	dd if=mbr.bin of=$(RUNDIR)/hd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader.bin of=$(RUNDIR)/hd.img bs=512 count=5 seek=1 conv=notrunc
	dd if=kernel.bin of=$(RUNDIR)/hd.img bs=512 count=145 seek=6 conv=notrunc

mbr.bin: $(SRCDIR)/boot/mbr.asm
	$(ASM_COMPILER) -o mbr.bin -f bin -I$(INCLUDE_PATH)/ $(SRCDIR)/boot/mbr.asm

bootloader.bin: $(SRCDIR)/boot/bootloader.asm
	$(ASM_COMPILER) -o bootloader.bin -f bin -I$(INCLUDE_PATH)/ $(SRCDIR)/boot/bootloader.asm

entry.obj: $(SRCDIR)/boot/entry.asm
	$(ASM_COMPILER) -o entry.obj -f elf32 $(SRCDIR)/boot/entry.asm

kernel.bin: kernel.o
	objcopy -O binary kernel.o kernel.bin

kernel.o: entry.obj $(OBJ)
	$(LINKER) -o kernel.o -melf_i386 -N entry.obj $(OBJ) -e enter_kernel -Ttext 0x00020000

$(CPP_OBJ):
	$(CPP_COMPILER) $(CPP_COMPILER_FLAGS) -I$(INCLUDE_PATH) -c $(CPP_SOURCE)

asm_utils.o: $(SRCDIR)/utils/asm_utils.asm
	$(ASM_COMPILER) -o asm_utils.o -f elf32 $(SRCDIR)/utils/asm_utils.asm

clean:
	rm -f *.o* *.bin

run:
	qemu-system-i386 -hda $(RUNDIR)/hd.img -serial null -parallel stdio -no-reboot

debug:
	qemu-system-i386 -S -s -parallel stdio -hda $(RUNDIR)/hd.img -serial null&
	@sleep 1
	gnome-terminal -e "gdb -q -tui -x $(RUNDIR)/gdbinit"
