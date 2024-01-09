#include "cpu.h"
#include "stivale2.h"
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define PIC1           0x20
#define PIC2           0xA0
#define PIC1_COMMAND   PIC1
#define PIC1_DATA      (PIC1 + 1)
#define PIC2_COMMAND   PIC2
#define PIC2_DATA      (PIC2 + 1)

extern void _load_gdt_and_tss_asm(struct GDT_Pointer *ptr);
extern void _load_idt_asm(struct IDT_Pointer *ptr);
extern uintptr_t _isr_names_asm[];

static struct TSS tss;
static struct GDT gdt;
static struct GDT_Pointer gdt_pointer;
static struct IDT_Descriptor idt[256];
static struct IDT_Pointer idt_pointer;
static IRQ_Handler irq_handlers[16];

static const char *exceptions[] =
{
	"Divide Error",
	"Debug Exception",
	"NMI Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode (Undefined Opcode)",
	"Device Not Available (No Math Coprocessor)",
	"Double Fault",
	"Coprocessor Segment Overrun (reserved)",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection",
	"Page Fault",
	"Intel reserved",
	"x87 FPU Floating-Point Error (Math Fault)",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Control Protection Exception",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved"
};

void gdt_init(void)
{
	uint64_t i, tss_base;

	gdt.null.limit_15_0 = 0;
	gdt.null.base_15_0 = 0;
	gdt.null.base_23_16 = 0;
	gdt.null.type = 0x00;
	gdt.null.limit_19_16_and_flags = 0x00;
	gdt.null.base_31_24 = 0;

	gdt.kernel_code.limit_15_0 = 0;
	gdt.kernel_code.base_15_0 = 0;
	gdt.kernel_code.base_23_16 = 0;
	gdt.kernel_code.type = 0x9A;
	gdt.kernel_code.limit_19_16_and_flags = 0xA0;
	gdt.kernel_code.base_31_24 = 0;

	gdt.kernel_data.limit_15_0 = 0;
	gdt.kernel_data.base_15_0 = 0;
	gdt.kernel_data.base_23_16 = 0;
	gdt.kernel_data.type = 0x92;
	gdt.kernel_data.limit_19_16_and_flags = 0xA0;
	gdt.kernel_data.base_31_24 = 0;

	gdt.null2.limit_15_0 = 0;
	gdt.null2.base_15_0 = 0;
	gdt.null2.base_23_16 = 0;
	gdt.null2.type = 0x00;
	gdt.null2.limit_19_16_and_flags = 0x00;
	gdt.null2.base_31_24 = 0;

	gdt.user_data.limit_15_0 = 0;
	gdt.user_data.base_15_0 = 0;
	gdt.user_data.base_23_16 = 0;
	gdt.user_data.type = 0x92;
	gdt.user_data.limit_19_16_and_flags = 0xA0;
	gdt.user_data.base_31_24 = 0;

	gdt.user_code.base_15_0 = 0;
	gdt.user_code.base_23_16 = 0;
	gdt.user_code.type = 0x9A;
	gdt.user_code.limit_19_16_and_flags = 0xA0;
	gdt.user_code.base_31_24 = 0;

	gdt.ovmf_data.base_23_16 = 0;
	gdt.ovmf_data.type = 0x92;
	gdt.ovmf_data.limit_19_16_and_flags = 0xA0;
	gdt.ovmf_data.base_31_24 = 0;

	gdt.ovmf_code.limit_15_0 = 0;
	gdt.ovmf_code.base_15_0 = 0;
	gdt.ovmf_code.base_23_16 = 0;
	gdt.ovmf_code.type = 0x9A;
	gdt.ovmf_code.limit_19_16_and_flags = 0xA0;
	gdt.ovmf_code.base_31_24 = 0;

	gdt.tss_low.base_15_0 = 0;
	gdt.tss_low.base_23_16 = 0;
	gdt.tss_low.type = 0x89;
	gdt.tss_low.limit_19_16_and_flags = 0xA0;
	gdt.tss_low.base_31_24 = 0;

	gdt.tss_high.limit_15_0 = 0;
	gdt.tss_high.base_15_0 = 0;
	gdt.tss_high.base_23_16 = 0;
	gdt.tss_high.type = 0x00;
	gdt.tss_high.limit_19_16_and_flags = 0x00;
	gdt.tss_high.base_31_24 = 0;

	for(i = 0; i < sizeof(tss); ++i)
	{
		((uint8_t *)(void *)&tss)[i] = 0;
	}

	tss_base = (uint64_t)&tss;
	gdt.tss_low.base_15_0 = tss_base & 0xFFFF;
	gdt.tss_low.base_23_16 = (tss_base >> 16) & 0xFF;
	gdt.tss_low.base_31_24 = (tss_base >> 24) & 0xFF;
	gdt.tss_low.limit_15_0 = sizeof(tss);
	gdt.tss_high.limit_15_0 = (tss_base >> 32) & 0xFFFF;
	gdt.tss_high.base_15_0 = (tss_base >> 48) & 0xFFFF;

	gdt_pointer.limit = sizeof(gdt) - 1;
	gdt_pointer.base = (uint64_t)&gdt;

	_load_gdt_and_tss_asm(&gdt_pointer);
	printf("GDT initialized\n");
}

static void create_descriptor(uint8_t index)
{
	uint64_t offset = _isr_names_asm[index];
	idt[index].offset_15_0 = offset & 0xFFFF;
	idt[index].selector = 0x08;
	idt[index].ist = 0;
	idt[index].type_and_attributes = 0x8E;
	idt[index].offset_31_16 = (offset >> 16) & 0xFFFF;
	idt[index].offset_63_32 = (offset >> 32) & 0xFFFFFFFF;
	idt[index].zero = 0;
}

static void io_wait(void)
{
	inb(0x80);
}

void idt_init(void)
{
	uint32_t i;
	for(i = 0; i <= 31; ++i)
	{
		create_descriptor(i);
	}

	outb(PIC1_COMMAND, 0x11);
	outb(PIC2_COMMAND, 0x11);
	io_wait();

	outb(PIC1_DATA, 0x20);
	outb(PIC2_DATA, 0x28);
	io_wait();

	outb(PIC1_DATA, 0x04);
	outb(PIC2_DATA, 0x02);
	io_wait();

	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);
	io_wait();

	outb(PIC1_DATA, 0x00);
	outb(PIC2_DATA, 0x00);
	io_wait();

	for(i = 32; i <= 47; ++i)
	{
		create_descriptor(i);
	}

	create_descriptor(255);

	idt_pointer.limit = sizeof(idt) - 1;
	idt_pointer.base = (uint64_t)&idt;

	_load_idt_asm(&idt_pointer);
	printf("IDT initialized\n");
}

void isr_register(uint32_t id, IRQ_Handler handler)
{
	irq_handlers[id] = handler;
}

uint64_t isr_handler(uint64_t rsp)
{
	interrupt_cpu_state_t *cpu = (interrupt_cpu_state_t *)rsp;

	if(cpu->isr_number <= 31)
	{
		printf(
			"\n"
			"-------------------\n"
			"EXCEPTION OCCURRED!\n\n"
			"ISR-No. %"PRId64": %s\n"
			"Error code: 0x%0"PRIx64"\n\n\n"
			"Register dump:\n\n",
				cpu->isr_number, exceptions[cpu->isr_number],
				cpu->error_code);

		printf(
			"  rax: 0x%016"PRIx64"   rbx:    0x%016"PRIx64"   rcx: 0x%016"PRIx64"   rdx: 0x%016"PRIx64"\n"
			"  rsi: 0x%016"PRIx64"   rdi:    0x%016"PRIx64"   rbp: 0x%016"PRIx64"   r8:  0x%016"PRIx64"\n"
			"  r9:  0x%016"PRIx64"   r10:    0x%016"PRIx64"   r11: 0x%016"PRIx64"   r12: 0x%016"PRIx64"\n"
			"  r13: 0x%016"PRIx64"   r14:    0x%016"PRIx64"   r15: 0x%016"PRIx64"   ss:  0x%016"PRIx64"\n"
			"  rsp: 0x%016"PRIx64"   rflags: 0x%016"PRIx64"   cs   0x%016"PRIx64"   rip: 0x%016"PRIx64"\n",
			cpu->rax, cpu->rbx,    cpu->rcx, cpu->rdx,
			cpu->rsi, cpu->rdi,    cpu->rbp, cpu->r8,
			cpu->r9,  cpu->r10,    cpu->r11, cpu->r12,
			cpu->r13, cpu->r14,    cpu->r15, cpu->ss,
			cpu->rsp, cpu->rflags, cpu->cs,  cpu->rip);

		asm volatile("cli; hlt");
	}
	else if(cpu->isr_number >= 32 && cpu->isr_number <= 47)
	{
		IRQ_Handler handler;
		if(cpu->isr_number >= 40)
		{
			outb(PIC2_COMMAND, 0x20);
		}

		outb(PIC1_COMMAND, 0x20);
		handler = irq_handlers[cpu->isr_number - 32];
		if(handler)
		{
			handler();
		}
	}

	return rsp;
}

static const char *get_memory_map_entry_type(uint32_t type)
{
	switch(type)
	{
	case STIVALE2_MMAP_USABLE:
		return "Usable";

	case STIVALE2_MMAP_RESERVED:
		return "Reserved";

	case STIVALE2_MMAP_ACPI_RECLAIMABLE:
		return "ACPI Reclaimable";

	case STIVALE2_MMAP_ACPI_NVS:
		return "ACPI Non Volatile Storage";

	case STIVALE2_MMAP_BAD_MEMORY:
		return "Bad Memory";

	case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
		return "Bootloader Reclaimable";

	case STIVALE2_MMAP_KERNEL_AND_MODULES:
		return "Kernel and Modules";

	case STIVALE2_MMAP_FRAMEBUFFER:
		return "Framebuffer";

	default:
		return "Unknown";
	}
}

void pmm_init(struct stivale2_struct *s)
{
	size_t i;
	size_t largest = 0;
	uintptr_t start = 0;
	struct stivale2_struct_tag_memmap *memory_map;
	memory_map = stivale2_get_tag(s, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	for(i = 0; i < memory_map->entries; ++i)
	{
		struct stivale2_mmap_entry *cur = &memory_map->memmap[i];
		if(cur->type == STIVALE2_MMAP_USABLE && cur->length > largest)
		{
			largest = cur->length;
			start = cur->base;
		}
	}

	if(start)
	{
		allocator_init(start, largest);
	}
}

void memory_map_print(struct stivale2_struct *s)
{
	uint64_t i;
	struct stivale2_struct_tag_memmap *memory_map;
	memory_map = stivale2_get_tag(s, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	printf("Memory map layout:\n"
		"       Base:                Size:                Type:\n");
	for(i = 0; i < memory_map->entries; ++i)
	{
		struct stivale2_mmap_entry *cur = &memory_map->memmap[i];
		printf("%4"PRId64":  0x%016"PRIx64" | 0x%016"PRIx64" | %s\n",
			i, cur->base, cur->length, get_memory_map_entry_type(cur->type));
	}
}
