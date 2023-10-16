# osdev-x64

## Run the project

Builds Kernel to .elf file:
```shell
make
```

Create `image.iso` file (runs make internally)
```shell
./iso.sh
```

Runs the qemu emulator
```shell
./emu.sh
```

## Directory Structure (that we want)

```
- iso_root
    - myos.elf
- limine
    (limine stuff)
- kernel
    - Makefile
    - kernel.c
    (Kernel stuff)
- modules
    - shell
        - Makefile
        - shell.c
        - shell.bin (Output binary)
    - fs
    (Kernel Modules)
- Makefile (builds all modules by calling their Makefile)
- iso.sh (copies the output binarys to the iso_root folder)
- emu.sh (opens iso with qemu)
- myos.iso
```