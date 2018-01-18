#!Makefile
#
#

# patsubst 处理所有在 C_SOURCES 字列中的字（一列文件名），如果它的 结尾是 '.c'，就用 '.o' 把 '.c' 取代
C_SOURCES = $(shell find . -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES = $(shell find . -name "*.s")
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))

CC = gcc
LD = ld
ASM = nasm

C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector -I include
LD_FLAGS = -T scripts/kernel.ld -m elf_i386 -nostdlib
ASM_FLAGS = -f elf -g -F stabs

all: $(S_OBJECTS) $(C_OBJECTS) link update_image

# The automatic variable `$<' is just the first prerequisite
.c.o:
	$(CC) $(C_FLAGS) $< -o $@

.s.o:
	$(ASM) $(ASM_FLAGS) $<

link:
	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o toyos_kernel

.PHONY:clean
clean:
	@$(RM) $(S_OBJECTS) $(C_OBJECTS) hx_kernel

.PHONY:update_image
update_image:
	mount floppy.img /mnt/kernel
	cp toyos_kernel /mnt/kernel/hx_kernel
	umount /mnt/kernel

.PHONY:mount_image
mount_image:
	@mount floppy.img /mnt/kernel

.PHONY:umount_image
umount_image:
	@umount /mnt/kernel

.PHONY:qemu
qemu:
	@qemu -fda floppy.img -boot a

.PHONY:debug
debug:
	@qemu -S -s -fda floppy.img -boot a &
	@gdb -tui -x scripts/gdbinit

