#include "cpu.h"
#include "stivale2.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PIC1           0x20
#define PIC2           0xA0
#define PIC1_COMMAND   PIC1
#define PIC1_DATA      (PIC1 + 1)
#define PIC2_COMMAND   PIC2
#define PIC2_DATA      (PIC2 + 1)
#define ICW4_8086      0x01

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

	for (uint64_t i = 0; i < sizeof(tss); i++)
		((uint8_t *)(void *)&tss)[i] = 0;

	uint64_t tss_base = (uint64_t)&tss;

	gdt.tss_low.base_15_0 = tss_base & 0xFFFF;
	gdt.tss_low.base_23_16 = (tss_base >> 16) & 0xFF;
	gdt.tss_low.base_31_24 = (tss_base >> 24) & 0xFF;
	gdt.tss_low.limit_15_0 = sizeof(tss);
	gdt.tss_high.limit_15_0 = (tss_base >> 32) & 0xFFFF;
	gdt.tss_high.base_15_0 = (tss_base >> 48) & 0xFFFF;

	gdt_pointer.limit = sizeof(gdt) - 1;
	gdt_pointer.base = (uint64_t)&gdt;

	_load_gdt_and_tss_asm(&gdt_pointer);
	printk("GDT initialized\n");
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

void idt_init(void)
{
	int i;
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
	printk("IDT initialized\n");
}

void pic_disable(void)
{
	outb(PIC2_DATA, 0xFF);
	outb(PIC1_DATA, 0xFF);
}

void isr_register(int id, IRQ_Handler handler)
{
	irq_handlers[id] = handler;
}

uint64_t isr_handler(uint64_t rsp)
{
	interrupt_cpu_state_t *cpu = (interrupt_cpu_state_t *)rsp;

	if(cpu->isr_number <= 31)
	{
		printk(
			"\n"
			"-------------------\n"
			"EXCEPTION OCCURRED!\n\n"
			"ISR-No. %d: %s\n"
			"Error code: 0x%.16llx\n\n\n"
			"Register dump:\n\n"
			"  rax: 0x%.16llx   rbx:    0x%.16llx   rcx: 0x%.16llx   rdx: 0x%.16llx\n"
			"  rsi: 0x%.16llx   rdi:    0x%.16llx   rbp: 0x%.16llx   r8:  0x%.16llx\n"
			"  r9:  0x%.16llx   r10:    0x%.16llx   r11: 0x%.16llx   r12: 0x%.16llx\n"
			"  r13: 0x%.16llx   r14:    0x%.16llx   r15: 0x%.16llx   ss:  0x%.16llx\n"
			"  rsp: 0x%.16llx   rflags: 0x%.16llx   cs   0x%.16llx   rip: 0x%.16llx\n",
			cpu->isr_number, exceptions[cpu->isr_number],
			cpu->error_code,
			cpu->rax, cpu->rbx,    cpu->rcx, cpu->rdx,
			cpu->rsi, cpu->rdi,    cpu->rbp, cpu->r8,
			cpu->r9,  cpu->r10,    cpu->r11, cpu->r12,
			cpu->r13, cpu->r14,    cpu->r15, cpu->ss,
			cpu->rsp, cpu->rflags, cpu->cs,  cpu->rip);

		for(;;)
		{
			asm volatile("cli; hlt");
		}
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
		return "Kernel And Modules";

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
	printk("Memory map layout:\n");
	for(i = 0; i < memory_map->entries; ++i)
	{
		struct stivale2_mmap_entry *cur = &memory_map->memmap[i];

		if(cur->type == STIVALE2_MMAP_USABLE && cur->length > largest)
		{
			largest = cur->length;
			start = cur->base;
		}

		printk("%.8d: Base: 0x%.16llx | Length: 0x%.16llx | Type: %s\n",
			i, cur->base, cur->length, get_memory_map_entry_type(cur->type));
	}

	if(start)
	{
		allocator_init(start, largest);
	}
}
