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

static void boot_any_key(int key, int ascii, int released)
{
	if(ascii == '\n')
	{
	graphics_rect(10, 10, 10, 10, GFX_WHITE);

		//printk("Enter pressed\n");
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

	graphics_init(stivale2_struct, GFX_BLACK);
	graphics_rect(50, 50, 200, 200, GFX_RED);
	serial_init();
	printk("Framebuffer and serial initialized\n");

	gdt_init();
	idt_init();
	printk("CPU Vendor ID String: %s\n", cpu_get_vendor_string());
	//acpi_init(stivale2_struct);
	//apic_init();
	keyboard_init();
	mouse_init();
	cursor_init();
	rtc_read();
	printk("Press enter to continue\n");

#if 0
	uint64_t addr = checkAllBuses();
	printk("%016x\n", addr);
	HBA_MEM *abar = (HBA_MEM *)addr;
	struct port_data pdtable[32];
	probe_port(abar, pdtable);

	graphics_char(10, 10, 'A', GFX_RED, GFX_BLACK, 0);

	//char buf[4096];
	//printk("RES = %d\n", read_sata(&pdtable[0], 0, 0, 1, buf));

    //for (i = 0;i < TOTAL_BLOCK_COUNT;++i)
    //    write_sata(pdtable[0], i, 0, 1, common_buf);
#endif

	keyboard_event_register(boot_any_key);
	mouse_event_register(mouse_event);
	halt();
}
