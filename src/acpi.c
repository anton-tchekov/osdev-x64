#include <stddef.h>
#include <stddef.h>
#include "stivale2.h"
#include "acpi.h"
#include "stdio.h"
#include "string.h"
#include "cpu.h"

static rsdt_structure_t *rsdt;

void acpi_init(struct stivale2_struct *stivale2_struct)
{
	struct stivale2_struct_tag_rsdp *rsdp_tag = stivale2_get_tag(stivale2_struct,
			STIVALE2_STRUCT_TAG_RSDP_ID);

	rsdp_init(rsdp_tag->rsdp);
	rsdt = (rsdt_structure_t *)phys_to_higher_half_data((uintptr_t)get_rsdp_structure()->rsdt_address);
	if(acpi_check_sdt_header(&rsdt->header, "RSDT") != 0)
	{
		kernel_log(ERROR, "No ACPI was found on this computer!\n");
		kernel_log(ERROR, "Kernel halted!\n");

		for (;;)
			asm ("hlt");
	}

	madt_init();
	kernel_log(INFO, "ACPI initialized\n");
}

int acpi_check_sdt_header(sdt_header_t *sdt_header, const char *signature)
{
	if (memcmp(sdt_header->signature, signature, 4) == 0 &&
			acpi_verify_sdt_header_checksum(sdt_header, signature) == 0)
		return 0;

	return 1;
}

int acpi_verify_sdt_header_checksum(sdt_header_t *sdt_header, const char *signature)
{
	uint8_t checksum = 0;
	uint8_t *ptr = (uint8_t *)sdt_header;
	uint8_t current_byte;
	kernel_log(INFO, "Verifying %s checksum:\n", signature);
	serial_tx_str(TERMINAL_PURPLE);
	debug("First %d bytes are being checked: ", sdt_header->length);
	printk(GFX_PURPLE, "First %d bytes are being checked: ", sdt_header->length);

	for (uint8_t i = 0; i < sdt_header->length; i++)
	{
		current_byte = ptr[i];
		debug("%x ", current_byte);
		printk(GFX_PURPLE, "%x ", current_byte);

		checksum += current_byte;
	}

	debug("\n");
	printk(GFX_PURPLE, "\n");

	serial_tx_str(TERMINAL_RESET);

	checksum = checksum & 0xFF;

	if (checksum == 0)
	{
		kernel_log(INFO, "%s checksum is verified.\n", signature);

		return 0;
	}
	else
	{
		kernel_log(ERROR, "%s checksum isn't 0! Checksum: 0x%x\n", signature, checksum);

		return 1;
	}
}

sdt_header_t *acpi_find_sdt_table(const char *signature)
{
	size_t entry_count = (rsdt->header.length - sizeof(rsdt->header)) / (has_xsdt() ? 8 : 4);
	sdt_header_t *current_entry;

	for (size_t i = 0; i < entry_count; i++)
	{
		current_entry = (sdt_header_t *)(uintptr_t)rsdt->entries[i];

		if (acpi_check_sdt_header(current_entry, signature) == 0)
			return (sdt_header_t *)phys_to_higher_half_data((uintptr_t)current_entry);
	}

	kernel_log(ERROR, "Could not find SDT with signature '%s'!\n", signature);
	return NULL;
}

static rsdp_structure_t *rsdp;
static bool has_xsdt_var = false;

void rsdp_init(uint64_t rsdp_address)
{
	rsdp_verify_checksum(rsdp_address);
	rsdp = (rsdp_structure_t *)rsdp_address;
	serial_tx_str(TERMINAL_PURPLE);
	if(rsdp->revision >= 2)
	{
		has_xsdt_var = true;
		kernel_log(INFO, "ACPI Version 2.0 or above is used\n");
	}
	else
	{
		kernel_log(INFO, "ACPI Version 1.0 is used\n");
	}

	serial_tx_str(TERMINAL_RESET);
}

void rsdp_verify_checksum(uint64_t rsdp_address)
{
	uint8_t checksum = 0;
	uint8_t *ptr = (uint8_t *)rsdp_address;
	uint8_t current_byte;
	kernel_log(INFO, "Verifying RSDP checksum:\n");
	serial_tx_str(TERMINAL_PURPLE);
	debug("20 first bytes are being checked: ");
	printk(GFX_PURPLE, "20 first bytes are being checked: ");
	for (uint8_t i = 0; i < 20; i++)
	{
		current_byte = ptr[i];
		debug("%x ", current_byte);
		printk(GFX_PURPLE, "%x ", current_byte);
		checksum += current_byte;
	}

	debug("\n");
	printk(GFX_PURPLE, "\n");
	serial_tx_str(TERMINAL_RESET);
	if ((checksum & 0xFF) == 0x00)
	{
		kernel_log(INFO, "RSDP checksum is verified\n");
	}
	else
	{
		kernel_log(ERROR, "RSDP checksum isn't 0! Checksum: 0x%x\n", checksum & 0xFF);
		kernel_log(ERROR, "Kernel halted!\n");
		halt();
	}
}

rsdp_structure_t *get_rsdp_structure(void)
{
	return rsdp;
}

bool has_xsdt(void)
{
	return has_xsdt_var;
}

madt_structure_t *madt;

madt_lapic_t *madt_lapics[64];
madt_io_apic_t *madt_io_apics[64];
madt_iso_t *madt_isos[64];
madt_lapic_nmi_t *madt_lapic_nmis[64];

size_t madt_lapics_i = 0;
size_t madt_io_apics_i = 0;
size_t madt_isos_i = 0;
size_t madt_lapic_nmis_i = 0;

void madt_init(void)
{
	madt = (madt_structure_t *)(uintptr_t)acpi_find_sdt_table("APIC");
	if(madt == NULL)
	{
		kernel_log(ERROR, "No MADT was found on this computer!\n");
		kernel_log(ERROR, "Kernel halted!\n");
		halt();
	}

	size_t madt_table_length = (size_t)&madt->header + madt->header.length;
	uint8_t *table_ptr = (uint8_t *)&madt->table;
	while((size_t)table_ptr < madt_table_length)
	{
		switch(*table_ptr)
		{
		case PROCESSOR_LOCAL_APIC:
			kernel_log(INFO, "MADT Initialization: Found local APIC\n");
			madt_lapics[madt_lapics_i++] = (madt_lapic_t *)table_ptr;
			break;

		case IO_APIC:
			kernel_log(INFO, "MADT Initialization: Found IO APIC\n");
			madt_io_apics[madt_io_apics_i++] = (madt_io_apic_t *)table_ptr;
			break;

		case INTERRUPT_SOURCE_OVERRIDE:
			kernel_log(INFO, "MADT Initialization: Found interrupt source override\n");
			madt_isos[madt_isos_i++] = (madt_iso_t *)table_ptr;
			break;

		case LAPIC_NMI:
			kernel_log(INFO, "MADT Initialization: Found local APIC non maskable interrupt\n");
			madt_lapic_nmis[madt_lapic_nmis_i++] = (madt_lapic_nmi_t *)table_ptr;
			break;
		}

		table_ptr += *(table_ptr + 1);
	}
}

uintptr_t lapic_base;

void apic_init(void)
{
	if(!apic_is_available())
	{
		kernel_log(ERROR, "No APIC was found on this computer!\n");
		kernel_log(ERROR, "Kernel halted!\n");
		halt();
	}

	lapic_base = phys_to_higher_half_data((uintptr_t)madt->lapic_address);
	pic_remap();
	pic_disable();
	lapic_enable();
}

bool apic_is_available(void)
{
	cpuid_registers_t *regs = &(cpuid_registers_t)
	{
		.leaf = CPUID_GET_FEATURES,
		.subleaf = 0,

		.eax = 0,
		.ebx = 0,
		.ecx = 0,
		.edx = 0
	};

	cpuid(regs);
	return (regs->edx & CPUID_FEAT_EDX_APIC);
}

uint32_t lapic_read_register(uint32_t reg)
{
	return *((volatile uint32_t *)lapic_base + reg);
}

void lapic_write_register(uint32_t reg, uint32_t data)
{
	*((volatile uint32_t *)(lapic_base + reg)) = data;
}

void lapic_enable(void)
{
	lapic_write_register(APIC_SPURIOUS_VECTOR_REGISTER, APIC_SOFTWARE_ENABLE | SPURIOUS_INTERRUPT);
}

void lapic_signal_eoi(void)
{
	lapic_write_register(APIC_EOI_REGISTER, 0);
}

uint32_t io_apic_read_register(size_t io_apic_i, uint8_t reg_offset)
{
	uint32_t volatile *current_io_apic_base = (uint32_t volatile *)madt_io_apics[io_apic_i];
	*current_io_apic_base = reg_offset;
	return *(current_io_apic_base + 0x10);
}

void io_apic_write_register(size_t io_apic_i, uint8_t reg_offset, uint32_t data)
{
	uint32_t volatile *current_io_apic_base = (uint32_t volatile *)madt_io_apics[io_apic_i];
	*current_io_apic_base = reg_offset;
	*(current_io_apic_base + 0x10) = data;
}
