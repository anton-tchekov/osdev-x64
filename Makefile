TARGET		:= src/kernel.elf
ISO_IMAGE	:= disk.iso

CC	= gcc
AS	= @nasm
LD	= ld

CC_FLAGS	= -Wall -Wextra -O2 -pipe -ggdb
AS_FLAGS	= -felf64
LD_FLAGS	=

INTERNAL_LD_FLAGS :=		\
	-Tsrc/linker.ld	\
	-nostdlib				\
	-static					\
	-zmax-page-size=0x1000	\
	--no-dynamic-linker		\
	-ztext
	# -pie

INTERNAL_CC_FLAGS :=		\
	-Isrc/			\
	-std=gnu11				\
	-ffreestanding			\
	-fno-stack-protector	\
	-fno-pic -fpie			\
	-mno-80387				\
	-mno-mmx				\
	-mno-3dnow				\
	-mno-sse				\
	-mno-sse2				\
	-mno-red-zone

C_FILES		:= $(shell find src/ -type f -name '*.c')
AS_FILES	:= $(shell find src/ -type f -name '*.s')

C_OBJ	= $(C_FILES:.c=.o)
AS_OBJ	= $(AS_FILES:.s=.o)
OBJ		= $(C_OBJ) $(AS_OBJ)

.PHONY: all clean format run

all: $(TARGET)

run: $(ISO_IMAGE)
	qemu-system-x86_64 -M q35 -m 2G -serial stdio -cdrom $(ISO_IMAGE)

debug: $(ISO_IMAGE)
	qemu-system-x86_64 -M q35 -m 2G -serial stdio -cdrom $(ISO_IMAGE) -s -S

limine:
	make -C third_party/limine

$(TARGET): $(OBJ)
	$(LD) -r -b binary -o src/font_unifont.o sfn_fonts/unifont.sfn
	$(LD) src/font_unifont.o $(OBJ) $(LD_FLAGS) $(INTERNAL_LD_FLAGS) -o $@
	@printf "\n\n \(^_^)/ Kernel compiled and linked successfully \(^_^)/ \n\n"


$(ISO_IMAGE): limine $(TARGET)
	rm -rf iso_root
	mkdir -p iso_root
	cp $(TARGET) 												\
		limine.cfg third_party/limine/limine.sys				\
		third_party/limine/limine-cd.bin 						\
		third_party/limine/limine-eltorito-efi.bin iso_root/
	xorriso -as mkisofs -b limine-cd.bin							\
		-no-emul-boot -boot-load-size 4 -boot-info-table			\
		--efi-boot limine-eltorito-efi.bin							\
		-efi-boot-part --efi-boot-image --protective-msdos-label	\
		iso_root -o $(ISO_IMAGE)
	third_party/limine/limine-install $(ISO_IMAGE)
	rm -rf iso_root

%.o: %.c
	@printf " [CC]\t$<\n";
	$(CC) $(CC_FLAGS) $(INTERNAL_CC_FLAGS) -c $< -o $@

%.o: %.s
	@printf " [AS]\t$<\n";
	$(AS) $(AS_FLAGS) $< -o $@

clean:
	rm -rf $(TARGET) $(OBJ) $(ISO_IMAGE)

format:
	astyle --mode=c -nA1fpxgHxbxjxpS $(C_FILES)
