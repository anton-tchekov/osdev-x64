#ifndef __CPU_H__
#define __CPU_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stivale2.h"

typedef struct
{
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rbp;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;

	uint64_t isr_number;
	uint64_t error_code;

	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} interrupt_cpu_state_t;

struct GDT_Descriptor
{
	uint16_t limit_15_0;
	uint16_t base_15_0;
	uint8_t base_23_16;
	uint8_t type;
	uint8_t limit_19_16_and_flags;
	uint8_t base_31_24;
} __attribute__((packed));

struct TSS
{
	uint32_t reserved0;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t reserved1;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t iopb_offset;
} __attribute__((packed));

struct GDT
{
	struct GDT_Descriptor null;
	struct GDT_Descriptor kernel_code;
	struct GDT_Descriptor kernel_data;
	struct GDT_Descriptor null2;
	struct GDT_Descriptor user_data;
	struct GDT_Descriptor user_code;
	struct GDT_Descriptor ovmf_data;
	struct GDT_Descriptor ovmf_code;
	struct GDT_Descriptor tss_low;
	struct GDT_Descriptor tss_high;
} __attribute__((aligned(4096))) __attribute__((packed));

struct GDT_Pointer
{
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

struct IDT_Descriptor
{
	uint16_t offset_15_0;
	uint16_t selector;
	uint8_t ist;
	uint8_t type_and_attributes;
	uint16_t offset_31_16;
	uint32_t offset_63_32;
	uint32_t zero;
} __attribute__((packed));

struct IDT_Pointer
{
	uint16_t limit;
	uint64_t base;
} __attribute__((packed));

typedef void (*IRQ_Handler)(void);

static void outb(uint16_t port, uint8_t value)
{
	asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

void isr_register(int id, IRQ_Handler handler);
void gdt_init(void);
void idt_init(void);
void pic_remap(void);
void pmm_init(struct stivale2_struct *s);
void memory_map_print(struct stivale2_struct *s);

#endif
