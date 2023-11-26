#include "stivale2.h"
#include "graphics.h"
#include "serial.h"
#include "cpu.h"
#include "ps2.h"
#include "shell/shell.h"
#include <stdio.h>
#include "cmos.h"
#include "terminal.h"

static void boot_any_key(int key, int ascii, int released)
{
	if(ascii == '\n')
	{
		shell_init();
	}

    (void)key, (void)released;
}

void kmain(struct stivale2_struct *stivale2_struct)
{
	pmm_init(stivale2_struct);
	serial_init();
	graphics_init(stivale2_struct, GFX_BLACK);
	terminal_init(graphics_width(), graphics_height());
	printf("Framebuffer and serial initialized\n");
	memory_map_print(stivale2_struct);
	gdt_init();
	idt_init();
	printf("CPU Vendor ID String: %s\n", cpu_get_vendor_string());

	ps2_init();
    rtc_read();

	printf("Press enter to continue\n");
	keyboard_event_register(boot_any_key);
	halt();
}
