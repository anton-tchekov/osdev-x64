TARGET := kernel/kernel.elf
ISO_IMAGE := disk.iso

CC = @gcc
AS = @nasm
LD = @ld

CC_FLAGS = \
	-Wall \
	-Wextra \
	-Wshadow \
	-Werror=implicit-function-declaration \
	-Werror=discarded-qualifiers \
	-O2 \
	-pipe \
	-Ikernel/ \
	-std=gnu99 \
	-ffreestanding \
	-fno-stack-protector \
	-fno-pic -fpie \
	-mno-80387 \
	-mno-mmx \
	-mno-3dnow \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-pedantic-errors

AS_FLAGS = -felf64

LD_FLAGS = \
	-Tkernel/linker.ld \
	-nostdlib \
	-static \
	-zmax-page-size=0x1000 \
	--no-dynamic-linker \
	-ztext

C_FILES := $(shell find kernel/ -type f -name '*.c')
AS_FILES := $(shell find kernel/ -type f -name '*.s')

C_OBJ = $(C_FILES:.c=.o)
AS_OBJ = $(AS_FILES:.s=.o)
OBJ = $(C_OBJ) $(AS_OBJ)

.PHONY: all clean limine run

all: $(TARGET) $(ISO_IMAGE)

run: $(ISO_IMAGE)
	qemu-system-x86_64 -M q35 -m 2G -serial stdio -cdrom $(ISO_IMAGE)

limine:
	make -C limine

$(TARGET): $(OBJ)
	$(LD) $(OBJ) $(LD_FLAGS) -o $@
	@printf "\n\n=== Kernel compiled successfully ===\n\n"

$(ISO_IMAGE): limine $(TARGET)
	mkdir -p iso_root
	cp $(TARGET) \
		limine.cfg limine/limine.sys \
		limine/limine-cd.bin \
		limine/limine-eltorito-efi.bin iso_root/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-eltorito-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(ISO_IMAGE)
	limine/limine-install $(ISO_IMAGE)

%.o: %.c
	@printf " [CC] $<\n";
	$(CC) $(CC_FLAGS) -c $< -o $@

%.o: %.s
	@printf " [AS]\t$<\n";
	$(AS) $(AS_FLAGS) $< -o $@

clean:
	rm -rf $(TARGET) $(OBJ) $(ISO_IMAGE)
