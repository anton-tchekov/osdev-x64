#ifndef __CPU_H__
#define __CPU_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stivale2.h"

typedef void (*IRQ_Handler)(void);

static inline void outb(uint16_t port, uint8_t value)
{
	asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

void isr_register(uint32_t id, IRQ_Handler handler);
void gdt_init(void);
void idt_init(void);
void pmm_init(struct stivale2_struct *s);
void memory_map_print(struct stivale2_struct *s);

#endif
