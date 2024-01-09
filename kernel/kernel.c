#include "stivale2.h"
#include "graphics.h"
#include "serial.h"
#include "cpu.h"
#include "ps2.h"
#include <stdio.h>
#include "cmos.h"
#include "terminal.h"
#include "module.h"

static struct stivale2_struct *_s;

static void boot_any_key(uint32_t key, uint32_t ascii, uint32_t released)
{
	if(ascii == '\n')
	{
		module_list(_s);
		module_init(_s);
	}

	(void)key, (void)released;
}

void kmain(struct stivale2_struct *s)
{
	_s = s;
	pmm_init(s);
	serial_init();
	graphics_init(s, GFX_BLACK);
	terminal_init(graphics_width(), graphics_height());
	printf("Framebuffer and serial initialized\n");
	memory_map_print(s);
	gdt_init();
	idt_init();
	ps2_init();
	rtc_read();
	printf("Press enter to continue\n");
	keyboard_event_register(boot_any_key);
	for(;;)
	{
		asm("hlt");
	}
}
