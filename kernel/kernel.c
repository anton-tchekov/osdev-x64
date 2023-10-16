#include <stddef.h>
#include <stdint.h>
#include "stivale2.h"
#include "cpu.h"
#include "keyboard.h"
#include "acpi.h"
#include "shell.h"
#include "stdio.h"

static void boot_any_key(int key, int ascii)
{
	if(ascii == KEY_RETURN)
	{
		shell_screen_init();
	}
}

void kmain(struct stivale2_struct *stivale2_struct)
{
	framebuffer_init(stivale2_struct, GFX_BLACK);
	serial_init();
	kernel_log(INFO, "Framebuffer and serial initialized\n");
	pmm_init(stivale2_struct);
	gdt_init();
	idt_init();
	kernel_log(INFO, "CPU Vendor ID String: %s\n", cpu_get_vendor_string());
	acpi_init(stivale2_struct);
	apic_init();
	keyboard_init();
	kernel_log(INFO, "Press enter to continue\n");
	activate_keyboard_processing(boot_any_key);
	halt();
}
