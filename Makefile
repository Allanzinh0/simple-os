# $@ = Target file
# $< = First dependency
# $^ - All dependencies

C_SOURCES = $(wildcard src/kernel/*.c src/drivers/*.c src/cpu/*.c src/libc/*.c)
HEADERS = $(wildcard src/kernel/*.h src/drivers/*.h src/cpu/*.h src/libc/*.h)
OBJ = $(patsubst src/%.c,obj/%.o,$(C_SOURCES))
OBJ += obj/cpu/interrupt.o
BUILD_FOLDERS := obj/kernel obj/drivers obj/cpu obj/libc obj/boot dist/boot

# Change tgus if your cross-compiler is somewhere else
CC = gcc
GDB = gdb

# -g: Use debugging symbols in gcc
CFLAGS = -g -m32 -nostdlib -nostdinc -fno-builtin -fno-pic -fno-stack-protector -nostartfiles -nodefaultlibs \
				 -Wall -Wextra -Werror


# First rule is run by default
all: dist/os-image.bin

dist/os-image.bin: dist/boot/bootsect.bin dist/kernel.bin
	cat $^ > $@

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
dist/kernel.bin: obj/boot/kernel_entry.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
dist/kernel.elf: obj/boot/kernel_entry.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^

run: dist/os-image.bin
	qemu-system-x86_64 $<

# Open the connection to qemu and load our kernel-object file with symbols
debug: dist/os-image.bin dist/kernel.elf
	qemu-system-x86_64 -s -fda dist/os-image.bin -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file dist/kernel.elf"
	
# Generic rules for wildcards
obj/%.o: src/%.c ${BUILD_FOLDERS} ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

obj/%.o: src/%.asm | ${BUILD_FOLDERS}
	nasm $< -f elf -o $@

dist/%.bin: src/%.asm | ${BUILD_FOLDERS}
	nasm $< -f bin -o $@

$(BUILD_FOLDERS):
	mkdir -p $@

clean:
	rm -rf *.bin *.o *.elf dist obj
