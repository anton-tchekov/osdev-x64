#include "serial.h"
#include "cpu.h"

#define COM1 0x3f8

void serial_init(void)
{
	outb(COM1 + 1, 0x00);
	outb(COM1 + 3, 0x80);
	outb(COM1 + 0, 0x03);
	outb(COM1 + 1, 0x00);
	outb(COM1 + 3, 0x03);
	outb(COM1 + 2, 0xC7);
	outb(COM1 + 4, 0x0B);
}

uint32_t serial_rx(void)
{
	while((inb(COM1 + 5) & 1) == 0) {}
	return inb(COM1);
}

void serial_tx(uint32_t c)
{
	while((inb(COM1 + 5) & 0x20) == 0) {}
	outb(COM1, c);
}

void serial_tx_str(const char *s)
{
	uint32_t c;
	while((c = *s++))
	{
		serial_tx(c);
	}
}
