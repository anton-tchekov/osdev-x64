#include <stdint.h>
#include <stddef.h>
#include "stivale2.h"
#include "keyboard.h"
#include "cpu.h"
#include "stdio.h"
#include "string.h"

extern void _load_gdt_and_tss_asm(struct GDT_Pointer *ptr);

static struct TSS		tss;
static struct GDT		gdt;
static struct GDT_Pointer   gdt_pointer;

void create_descriptors(void)
{

	gdt.null.limit_15_0					= 0;
	gdt.null.base_15_0					= 0;
	gdt.null.base_23_16					= 0;
	gdt.null.type					= 0x00;
	gdt.null.limit_19_16_and_flags			= 0x00;
	gdt.null.base_31_24					= 0;

	gdt.kernel_code.limit_15_0				= 0;
	gdt.kernel_code.base_15_0				= 0;
	gdt.kernel_code.base_23_16				= 0;
	gdt.kernel_code.type				= 0x9A;
	gdt.kernel_code.limit_19_16_and_flags		= 0xA0;
	gdt.kernel_code.base_31_24				= 0;

	gdt.kernel_data.limit_15_0				= 0;
	gdt.kernel_data.base_15_0				= 0;
	gdt.kernel_data.base_23_16				= 0;
	gdt.kernel_data.type				= 0x92;
	gdt.kernel_data.limit_19_16_and_flags		= 0xA0;
	gdt.kernel_data.base_31_24				= 0;

	gdt.null2.limit_15_0				= 0;
	gdt.null2.base_15_0						= 0;
	gdt.null2.base_23_16					= 0;
	gdt.null2.type					= 0x00;
	gdt.null2.limit_19_16_and_flags			= 0x00;
	gdt.null2.base_31_24				= 0;

	gdt.user_data.limit_15_0				= 0;
	gdt.user_data.base_15_0				= 0;
	gdt.user_data.base_23_16				= 0;
	gdt.user_data.type					= 0x92;
	gdt.user_data.limit_19_16_and_flags			= 0xA0;
	gdt.user_data.base_31_24				= 0;

	gdt.user_code.base_15_0				= 0;
	gdt.user_code.base_23_16				= 0;
	gdt.user_code.type					= 0x9A;
	gdt.user_code.limit_19_16_and_flags			= 0xA0;
	gdt.user_code.base_31_24				= 0;

	gdt.ovmf_data.base_23_16				= 0;
	gdt.ovmf_data.type					= 0x92;
	gdt.ovmf_data.limit_19_16_and_flags			= 0xA0;
	gdt.ovmf_data.base_31_24				= 0;

	gdt.ovmf_code.limit_15_0				= 0;
	gdt.ovmf_code.base_15_0				= 0;
	gdt.ovmf_code.base_23_16				= 0;
	gdt.ovmf_code.type					= 0x9A;
	gdt.ovmf_code.limit_19_16_and_flags			= 0xA0;
	gdt.ovmf_code.base_31_24				= 0;

	gdt.tss_low.base_15_0				= 0;
	gdt.tss_low.base_23_16				= 0;
	gdt.tss_low.type					= 0x89;
	gdt.tss_low.limit_19_16_and_flags			= 0xA0;
	gdt.tss_low.base_31_24				= 0;

	gdt.tss_high.limit_15_0				= 0;
	gdt.tss_high.base_15_0				= 0;
	gdt.tss_high.base_23_16				= 0;
	gdt.tss_high.type					= 0x00;
	gdt.tss_high.limit_19_16_and_flags			= 0x00;
	gdt.tss_high.base_31_24				= 0;
}

void gdt_init(void)
{
	create_descriptors();

	for (uint64_t i = 0; i < sizeof(tss); i++)
		((uint8_t *)(void *)&tss)[i] = 0;

	uint64_t tss_base = ((uint64_t)&tss);

	gdt.tss_low.base_15_0   = tss_base		& 0xffff;
	gdt.tss_low.base_23_16  = (tss_base >> 16)	& 0xff;
	gdt.tss_low.base_31_24  = (tss_base >> 24)	& 0xff;
	gdt.tss_low.limit_15_0  = sizeof(tss);
	gdt.tss_high.limit_15_0 = (tss_base >> 32)	& 0xffff;
	gdt.tss_high.base_15_0  = (tss_base >> 48)	& 0xffff;

	gdt_pointer.limit	= sizeof(gdt) - 1;
	gdt_pointer.base	= (uint64_t)&gdt;

	_load_gdt_and_tss_asm(&gdt_pointer);
	kernel_log(INFO, "GDT initialized\n");
}

extern void		_load_idt_asm(struct IDT_Pointer *ptr);
extern uintptr_t	_isr_names_asm[];

static struct IDT_Descriptor	idt[256];
static struct IDT_Pointer	idt_pointer;

void create_descriptor(uint8_t index, uint8_t type_and_attributes)
{
	uint64_t offset = _isr_names_asm[index];

	idt[index].offset_15_0		= offset & 0xFFFF;
	idt[index].selector			= 0x08;
	idt[index].ist			= 0;
	idt[index].type_and_attributes  = type_and_attributes;
	idt[index].offset_31_16		= (offset >> 16) & 0xFFFF;
	idt[index].offset_63_32		= (offset >> 32) & 0xFFFFFFFF;
	idt[index].zero			= 0;
}

void idt_init(void)
{
	create_descriptor(0, 0x8E);
	create_descriptor(1, 0x8E);
	create_descriptor(2, 0x8E);
	create_descriptor(3, 0x8E);
	create_descriptor(4, 0x8E);
	create_descriptor(5, 0x8E);
	create_descriptor(6, 0x8E);
	create_descriptor(7, 0x8E);
	create_descriptor(8, 0x8E);
	create_descriptor(9, 0x8E);
	create_descriptor(10, 0x8E);
	create_descriptor(11, 0x8E);
	create_descriptor(12, 0x8E);
	create_descriptor(13, 0x8E);
	create_descriptor(14, 0x8E);
	create_descriptor(15, 0x8E);
	create_descriptor(16, 0x8E);
	create_descriptor(17, 0x8E);
	create_descriptor(18, 0x8E);
	create_descriptor(19, 0x8E);
	create_descriptor(20, 0x8E);
	create_descriptor(21, 0x8E);
	create_descriptor(22, 0x8E);
	create_descriptor(23, 0x8E);
	create_descriptor(24, 0x8E);
	create_descriptor(25, 0x8E);
	create_descriptor(26, 0x8E);
	create_descriptor(27, 0x8E);
	create_descriptor(28, 0x8E);
	create_descriptor(29, 0x8E);
	create_descriptor(30, 0x8E);
	create_descriptor(31, 0x8E);

	pic_remap();

	create_descriptor(32, 0x8E);
	create_descriptor(33, 0x8E);
	create_descriptor(34, 0x8E);
	create_descriptor(35, 0x8E);
	create_descriptor(36, 0x8E);
	create_descriptor(37, 0x8E);
	create_descriptor(38, 0x8E);
	create_descriptor(39, 0x8E);
	create_descriptor(40, 0x8E);
	create_descriptor(41, 0x8E);
	create_descriptor(42, 0x8E);
	create_descriptor(43, 0x8E);
	create_descriptor(44, 0x8E);
	create_descriptor(45, 0x8E);
	create_descriptor(46, 0x8E);
	create_descriptor(47, 0x8E);

	create_descriptor(255, 0x8E);

	pic_set_mask(1);

	idt_pointer.limit = sizeof(idt) - 1;
	idt_pointer.base = (uint64_t)&idt;

	_load_idt_asm(&idt_pointer);
	kernel_log(INFO, "IDT initialized\n");
}


#define PIC1		0x20
#define PIC2		0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define ICW4_8086	0x01

void pic_disable(void)
{
	outb(PIC2_DATA, 0xFF);
	outb(PIC1_DATA, 0xFF);
}

void pic_remap(void)
{
	uint8_t mask1 = inb(PIC1_DATA);
	uint8_t mask2 = inb(PIC2_DATA);

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

	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	outb(PIC1_DATA, mask1);
	outb(PIC2_DATA, mask2);
}

void pic_set_mask(uint8_t irq_line)
{
	uint16_t port;
	uint8_t value;

	if(irq_line < 8)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
		irq_line -= 8;
	}

	value = inb(port) | (1 << irq_line);
	outb(port, value);
}

void pic_clear_mask(uint8_t irq_line)
{
	uint16_t port;
	uint8_t value;

	if(irq_line < 8)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
		irq_line -= 8;
	}

	value = inb(port) & ~(1 << irq_line);
	outb(port, value);
}

void pic_signal_EOI(uint64_t isr_number)
{
	if(isr_number >= 40)
	{
		outb(PIC2_COMMAND, 0x20);
	}

	outb(PIC1_COMMAND, 0x20);
}

static const char *exceptions[] =
{
	"#DE: Divide Error",
	"#DB: Debug Exception",
	"-  : NMI Interrupt",
	"#BP: Breakpoint",
	"#OF: Overflow",
	"#BR: BOUND Range Exceeded",
	"#UD: Invalid Opcode (Undefined Opcode)",
	"#NM: Device Not Available (No Math Coprocessor)",
	"#DF: Double Fault",
	"- : Coprocessor Segment Overrun (reserved)",
	"#TS: Invalid TSS",
	"#NP: Segment Not Present",
	"#SS: Stack-Segment Fault",
	"#GP: General Protection",
	"#PF: Page Fault",
	"- : (Intel reserved. Do not use.)",
	"#MF: x87 FPU Floating-Point Error (Math Fault)",
	"#AC: Alignment Check",
	"#MC: Machine Check",
	"#XM: SIMD Floating-Point Exception",
	"#VE: Virtualization Exception",
	"#CP: Control Protection Exception",
	"- : Intel reserved. Do not use.",
	"- : Intel reserved. Do not use.",
	"- : Intel reserved. Do not use.",
	"- : Intel reserved. Do not use.",
	"- : Intel reserved. Do not use.",
	"- : Intel reserved. Do not use.",
	"- : Intel reserved. Do not use.",
	"- : Intel reserved. Do not use.",
	"- : Intel reserved. Do not use.",
	"- : Intel reserved. Do not use."
};

uint64_t isr_handler(uint64_t rsp)
{
	interrupt_cpu_state_t *cpu = (interrupt_cpu_state_t*)rsp;

	if(cpu->isr_number <= 31)
	{
		serial_tx_str(TERMINAL_RED);
		debug("\n────────────────────────\n");
		debug("⚠ EXCEPTION OCCURRED! ⚠\n\n");
		debug("⤷ ISR-No. %d: %s\n", cpu->isr_number, exceptions[cpu->isr_number]);
		debug("⤷ Error code: 0x%.16llx\n\n\n", cpu->error_code);
		serial_tx_str(TERMINAL_CYAN);
		debug("ℹ Register dump:\n\n");
		debug("⤷ rax: 0x%.16llx, rbx:	0x%.16llx, rcx: 0x%.16llx, rdx: 0x%.16llx\n"
			  "⤷ rsi: 0x%.16llx, rdi:	0x%.16llx, rbp: 0x%.16llx, r8 : 0x%.16llx\n"
			  "⤷ r9 : 0x%.16llx, r10:	0x%.16llx, r11: 0x%.16llx, r12: 0x%.16llx\n"
			  "⤷ r13: 0x%.16llx, r14:	0x%.16llx, r15: 0x%.16llx, ss : 0x%.16llx\n"
			  "⤷ rsp: 0x%.16llx, rflags: 0x%.16llx, cs : 0x%.16llx, rip: 0x%.16llx\n",
			  cpu->rax, cpu->rbx,	cpu->rcx, cpu->rdx,
			  cpu->rsi, cpu->rdi,	cpu->rbp, cpu->r8,
			  cpu->r9,  cpu->r10,	cpu->r11, cpu->r12,
			  cpu->r13, cpu->r14,	cpu->r15, cpu->ss,
			  cpu->rsp, cpu->rflags, cpu->cs,  cpu->rip);

		serial_tx_str(TERMINAL_RESET);

		framebuffer_reset_screen();
		printk(GFX_RED,		"\n────────────────────────\n");
		printk(GFX_RED,		"⚠ EXCEPTION OCCURRED! ⚠\n\n");
		printk(GFX_RED,		"⤷ ISR-No. %d: %s\n", cpu->isr_number, exceptions[cpu->isr_number]);
		printk(GFX_RED,		"⤷ Error code: 0x%.16llx\n\n\n", cpu->error_code);
		printk(GFX_CYAN,	"ℹ Register dump:\n\n");
		printk(GFX_CYAN,	"⤷ rax: 0x%.16llx, rbx:	0x%.16llx, rcx: 0x%.16llx, rdx: 0x%.16llx\n"
			   "⤷ rsi: 0x%.16llx, rdi:	0x%.16llx, rbp: 0x%.16llx, r8 : 0x%.16llx\n"
			   "⤷ r9 : 0x%.16llx, r10:	0x%.16llx, r11: 0x%.16llx, r12: 0x%.16llx\n"
			   "⤷ r13: 0x%.16llx, r14:	0x%.16llx, r15: 0x%.16llx, ss : 0x%.16llx\n"
			   "⤷ rsp: 0x%.16llx, rflags: 0x%.16llx, cs : 0x%.16llx, rip: 0x%.16llx\n",
			   cpu->rax, cpu->rbx,	cpu->rcx, cpu->rdx,
			   cpu->rsi, cpu->rdi,	cpu->rbp, cpu->r8,
			   cpu->r9,  cpu->r10,	cpu->r11, cpu->r12,
			   cpu->r13, cpu->r14,	cpu->r15, cpu->ss,
			   cpu->rsp, cpu->rflags, cpu->cs,  cpu->rip);

		for(;;)
		{
			asm volatile("cli; hlt");
		}
	}
	else if(cpu->isr_number >= 32 && cpu->isr_number <= 47)
	{
		if(cpu->isr_number == 33)
		{
			keyboard_irq_handler();
		}

		pic_signal_EOI(cpu->isr_number);
	}

	return rsp;
}

struct PMM_Info_Struct
{
	size_t memory_size;
	uint32_t max_pages;
	uint32_t used_pages;
	struct stivale2_struct_tag_memmap *memory_map;
};

struct PMM_Info_Struct pmm_info;

size_t highest_page;

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

void pmm_init(struct stivale2_struct *stivale2_struct)
{
	struct stivale2_struct_tag_memmap *memory_map = stivale2_get_tag(stivale2_struct,
			STIVALE2_STRUCT_TAG_MEMMAP_ID);

	pmm_info.memory_map = memory_map;

	struct stivale2_mmap_entry *current_entry;
	kernel_log(INFO, "Memory map layout:\n");
	size_t top = 0;

	for (uint64_t i = 0; i < pmm_info.memory_map->entries; i++)
	{
		current_entry = &pmm_info.memory_map->memmap[i];

		serial_tx_str(TERMINAL_PURPLE);

		debug("%.8d: Base: 0x%.16llx | Length: 0x%.16llx | Type: %s\n",
			  i, current_entry->base, current_entry->length, get_memory_map_entry_type(current_entry->type));
		printk(GFX_PURPLE, "%.8d: Base: 0x%.16llx | Length: 0x%.16llx | Type: %s\n",
			   i, current_entry->base, current_entry->length, get_memory_map_entry_type(current_entry->type));

		serial_tx_str(TERMINAL_RESET);

		if (current_entry->type != STIVALE2_MMAP_USABLE &&
				current_entry->type != STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE &&
				current_entry->type != STIVALE2_MMAP_KERNEL_AND_MODULES)
			continue;

		top = current_entry->base + current_entry->length;

		if (top > highest_page)
			highest_page = top;
	}

	pmm_info.memory_size = highest_page;
	pmm_info.max_pages = KB_TO_PAGES(pmm_info.memory_size);
	pmm_info.used_pages = pmm_info.max_pages;
	kernel_log(INFO, "Memory specifications:\n");
	serial_tx_str(TERMINAL_PURPLE);
	current_entry = &pmm_info.memory_map->memmap[0];
	debug("Total amount of memory: %d kB\n", current_entry->base + current_entry->length - 1);
	printk(GFX_PURPLE, "Total amount of memory: %d kB\n", current_entry->base + current_entry->length - 1);
	serial_tx_str(TERMINAL_RESET);
}
