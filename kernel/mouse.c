#include "mouse.h"
#include "types.h"
#include "cpu.h"
#include <stdio.h>

static u8 mouse_cycle;
static i8 mouse_byte[3];
static MouseEvent mouse_event;

#define MOUSE_PORT          0x60
#define MOUSE_STATUS        0x64
#define MOUSE_BIT_A         0x02
#define MOUSE_BIT_B         0x01
#define MOUSE_TIMEOUT       100000

static void mouse_wait_a_bit(void)
{
	u32 timeout = MOUSE_TIMEOUT;
	while(--timeout)
	{
		if(((inb(MOUSE_STATUS) & MOUSE_BIT_A)) == 0)
		{
			return;
		}
	}
}

static void mouse_wait_b_bit(void)
{
	u32 timeout = MOUSE_TIMEOUT;
	while(--timeout)
	{
		if((inb(MOUSE_STATUS) & MOUSE_BIT_B) == 1)
		{
			return;
		}
	}
}

static void mouse_write(u8 write)
{
	mouse_wait_a_bit();
	outb(MOUSE_STATUS, 0xD4);
	mouse_wait_a_bit();
	outb(MOUSE_PORT, write);
}

static u8 mouse_read(void)
{
	mouse_wait_b_bit();
	return inb(MOUSE_PORT);
}

void mouse_irq_handler(void)
{
	u8 status = inb(MOUSE_STATUS);
	while(status & MOUSE_BIT_B)
	{

		i8 mouse_in = inb(MOUSE_PORT);
		if(status & 0x20)
		{
			switch(mouse_cycle)
			{
			case 0:
				mouse_byte[0] = mouse_in;
				if(!(mouse_in & 0x08))
				{
					return;
				}

				++mouse_cycle;
				break;

			case 1:
				mouse_byte[1] = mouse_in;
				++mouse_cycle;
				break;

			case 2:
				mouse_byte[2] = mouse_in;
				if(mouse_byte[0] & 0x80 || mouse_byte[0] & 0x40)
				{
					break;
				}

				if(mouse_event)
				{
					mouse_event(mouse_byte[1], mouse_byte[2], mouse_byte[0]);
				}

				mouse_cycle = 0;
				break;
			}
		}

		status = inb(MOUSE_STATUS);
	}
}

void mouse_init(void)
{
	u8 status;
	mouse_wait_a_bit();
	outb(MOUSE_STATUS, 0xA8);
	mouse_wait_a_bit();
	outb(MOUSE_STATUS, 0x20);
	mouse_wait_b_bit();
	status = inb(0x60) | 0x02;
	mouse_wait_a_bit();
	outb(MOUSE_STATUS, 0x60);
	mouse_wait_a_bit();
	outb(MOUSE_PORT, status);
	mouse_write(0xF6);
	mouse_read();
	mouse_write(0xF4);
	mouse_read();
	isr_register(12, mouse_irq_handler);
	printk("Mouse driver initialized\n");
}

void mouse_event_register(MouseEvent handler)
{
	mouse_event = handler;
}
