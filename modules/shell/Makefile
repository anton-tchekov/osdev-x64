MODULE_NAME = shell

all:
	gcc -s -nostdlib -fno-stack-protector -T ../linker.ld main.c -O2 -Wall -Wextra -o $(MODULE_NAME)
	objcopy -O binary $(MODULE_NAME) -j .header -j .text -j .data -j .rodata $(MODULE_NAME).bin
	cp $(MODULE_NAME).bin ../../iso_root/
