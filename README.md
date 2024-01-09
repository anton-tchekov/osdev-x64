# osdev-x64

## Run the project

Builds OS to `.elf` and `.iso` file:
```shell
make
```

Run OS in QEMU
```shell
make run
```

Burn OS image to USB flash drive
```shell
./burn.sh /dev/sdX
```

## Directory Structure

```
- iso_root
- limine
- kernel
    - Makefile
    - kernel.c
    - etc.
- modules (Kernel Modules)
    - template
    - shell
        - Makefile
        - shell.c
        - shell.bin (Output binary)
- Makefile
- disk.iso (OS ISO image)
```