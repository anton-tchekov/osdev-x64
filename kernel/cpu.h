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

typedef struct
{
	uint32_t leaf;
	uint32_t subleaf;

	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
} cpuid_registers_t;

typedef enum
{
	CPUID_GET_VENDOR_STRING
} cpuid_requests_t;

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

static inline int cpuid(cpuid_registers_t *registers)
{
	uint32_t cpuid_max;

	asm volatile("cpuid"
		: "=a" (cpuid_max)
		: "a" (registers->leaf & 0x80000000)
		: "rbx", "rcx", "rdx");

	if(registers->leaf > cpuid_max)
	{
		return 0;
	}

	asm volatile("cpuid" :
		"=a" (registers->eax),
		"=b" (registers->ebx),
		"=c" (registers->ecx),
		"=d" (registers->edx) :
		"a" (registers->leaf),
		"c" (registers->subleaf));

	return 1;
}

static inline char *cpu_get_vendor_string(void)
{
	static char vendor_string[16];
	cpuid_registers_t regs =
	{
		.leaf = CPUID_GET_VENDOR_STRING,
		.subleaf = 0,
		.eax = 0,
		.ebx = 0,
		.ecx = 0,
		.edx = 0
	};

	cpuid(&regs);
	snprintf(vendor_string, 13, "%.4s%.4s%.4s",
		(char *)&regs.ebx, (char *)&regs.edx, (char *)&regs.ecx);

	return vendor_string;
}

static inline void hlt(void)
{
	asm("hlt");
}

static inline void cli(void)
{
	asm("cli");
}

static inline void halt(void)
{
	for(;;)
	{
		hlt();
	}
}

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

static inline void io_wait(void)
{
	inb(0x80);
}

void isr_register(int id, IRQ_Handler handler);
void gdt_init(void);
void idt_init(void);
void pic_remap(void);
void pmm_init(struct stivale2_struct *s);
void memory_map_print(struct stivale2_struct *s);

#endif
