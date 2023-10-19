#include <stddef.h>
#include <stdint.h>
#include "stivale2.h"
#include "graphics.h"
#include "serial.h"
#include "cpu.h"
#include "keyboard.h"
#include "mouse.h"
#include "cursor.h"
#include "acpi.h"
#include "shell/shell.h"
#include "stdio.h"
#include "cmos.h"
#include "ahci.h"
#include "terminal.h"
#include "window.h"

static void boot_any_key(int key, int ascii, int released)
{
	if(ascii == '\n')
	{
		shell_init();
	}
}

static void mouse_event(int dx, int dy, int buttons)
{
	cursor_update(dx, dy);
}

void kmain(struct stivale2_struct *stivale2_struct)
{
	pmm_init(stivale2_struct);
	serial_init();
	graphics_init(stivale2_struct, GFX_BLACK);
	terminal_init(graphics_width(), graphics_height());
	printk("Framebuffer and serial initialized\n");
	memory_map_print(stivale2_struct);
	gdt_init();
	idt_init();
	printk("CPU Vendor ID String: %s\n", cpu_get_vendor_string());
	// acpi_init(stivale2_struct);
	// apic_init();
	keyboard_init();
	mouse_init();
	cursor_init();
	rtc_read();

	Window window = { 300, 100, 400, 200, "Window 1" };
	//window_draw(&window);

	uint64_t addr = checkAllBuses();
	printk("%016x\n", addr);
	HBA_MEM *abar = (HBA_MEM *)addr;
	struct port_data pdtable[32];
	probe_port(abar, pdtable);

#if 0
	graphics_char(10, 10, 'A', GFX_RED, GFX_BLACK, 0);
	char buf[4096];
	printk("RES = %d\n", read_sata(&pdtable[0], 0, 0, 1, buf));
#endif

	printk("Press enter to continue\n");
	keyboard_event_register(boot_any_key);
	mouse_event_register(mouse_event);
	halt();
}
