#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>
#include <stdbool.h>

typedef struct __attribute__((__packed__))
{
	char	signature[4];
	uint32_t	length;
	uint8_t	revision;
	uint8_t	checksum;
	char	oem_string[6];
	char	oem_table_id[8];
	uint32_t	oem_revision;
	char	creator_id[4];
	uint32_t	creator_revision;
} sdt_header_t;

typedef struct __attribute__((__packed__))
{
	sdt_header_t header;
	uint32_t entries[];
} rsdt_structure_t;


typedef struct __attribute__((__packed__))
{

	char	signature[8];
	uint8_t 	checksum;
	char		oemid[6];
	uint8_t 	revision;
	uint32_t	rsdt_address;

	uint32_t	length;
	uint64_t	xsdt_address;
	uint8_t	extended_checksum;
	uint8_t	reserved[3];

} rsdp_structure_t;

void rsdp_init(uint64_t rsdp_address);
void rsdp_verify_checksum(uint64_t rsdp_address);
rsdp_structure_t *get_rsdp_structure(void);
bool has_xsdt(void);


typedef struct __attribute__((__packed__))
{
	uint8_t entry_type;
	uint8_t record_length;
} madt_record_table_entry_t;

typedef enum
{
	PROCESSOR_LOCAL_APIC = 0x0,
	IO_APIC = 0x1,
	INTERRUPT_SOURCE_OVERRIDE = 0x2,
	LAPIC_NMI = 0x4,
} madt_entry_type_t;

typedef struct __attribute__((__packed__))
{
	madt_record_table_entry_t record;
	uint8_t acpi_processor_id;
	uint8_t acpi_id;
	uint32_t flags;
} madt_lapic_t;

typedef struct __attribute__((__packed__))
{
	madt_record_table_entry_t record;
	uint8_t io_apic_id;
	uint8_t reserved;
	uint8_t io_apic_address;
	uint8_t global_system_interrupt_base;
} madt_io_apic_t;

typedef struct __attribute__((__packed__))
{
	madt_record_table_entry_t record;
	uint8_t bus_source;
	uint8_t irq_source;
	uint32_t global_system_interrupt;
	uint16_t flags;
} madt_iso_t;

typedef struct __attribute__((__packed__))
{
	madt_record_table_entry_t record;
	uint8_t acpi_processor_id;
	uint16_t flags;
	uint8_t lint;
} madt_lapic_nmi_t;

typedef struct __attribute__((__packed__))
{
	sdt_header_t header;
	uint32_t lapic_address;
	uint32_t flags;
	madt_record_table_entry_t table[];
} madt_structure_t;

extern madt_structure_t *madt;

extern madt_lapic_t	*madt_lapics[];
extern madt_io_apic_t	*madt_io_apics[];
extern madt_iso_t	*madt_isos[];
extern madt_lapic_nmi_t	*madt_lapic_nmis[];

extern size_t madt_lapics_i;
extern size_t madt_io_apics_i;
extern size_t madt_isos_i;
extern size_t madt_lapic_nmis_i;

void madt_init(void);


void acpi_init(struct stivale2_struct *stivale2_struct);
int acpi_check_sdt_header(sdt_header_t *sdt_header, const char *signature);
int acpi_verify_sdt_header_checksum(sdt_header_t *sdt_header, const char *signature);
sdt_header_t *acpi_find_sdt_table(const char *signature);

#define APIC_SPURIOUS_VECTOR_REGISTER	0xF
#define APIC_EOI_REGISTER		0xB0
#define APIC_SOFTWARE_ENABLE		(1 << 8)

void apic_init(void);
bool apic_is_available(void);
uint32_t lapic_read_register(uint32_t reg);
void lapic_write_register(uint32_t reg, uint32_t data);
void lapic_enable(void);
uint32_t io_apic_read_register(size_t io_apic_i, uint8_t reg_offset);
void io_apic_write_register(size_t io_apic_i, uint8_t reg_offset, uint32_t data);

#endif
