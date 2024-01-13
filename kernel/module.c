#include "module.h"
#include "ps2.h"
#include "terminal.h"
#include "graphics.h"
#include "cpu.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

static const uint64_t functions[] =
{
	(uint64_t)printf,
	(uint64_t)terminal_set_color,
	(uint64_t)graphics_rect,
};

static ModuleSection *get_section(ModuleHeader *header, ModuleSectionType type)
{
	uint32_t i;
	for(i = 0; i < header->NumSections; ++i)
	{
		if(header->Sections[i].Type == type)
		{
			return &header->Sections[i];
		}
	}

	return NULL;
}

static char *get_section_str(ModuleHeader *header, ModuleSectionType type)
{
	return (char *)header + get_section(header, type)->Start;
}

void module_list(struct stivale2_struct *s)
{
	uint64_t i;
	struct stivale2_struct_tag_memmap *memory_map;
	memory_map = stivale2_get_tag(s, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	printf("Module list:\n");
	for(i = 0; i < memory_map->entries; ++i)
	{
		ModuleHeader *header;
		struct stivale2_mmap_entry *cur = &memory_map->memmap[i];
		if(cur->type != STIVALE2_MMAP_KERNEL_AND_MODULES)
		{
			continue;
		}

		header = (ModuleHeader *)cur->base;
		if(header->Magic != MODULE_MAGIC)
		{
			continue;
		}

		printf("Module Id: %"PRId32"; Type: %"PRId32"; NumSections: %"PRId32";\n"
				"    Address = %016"PRIX64"; Size: %016"PRIX64"\n",
				header->Id, header->Type,
				header->NumSections, cur->base, cur->length);

		printf("Author: %s\nName: %s\nDescription: %s\n",
				get_section_str(header, MODULE_SECTION_AUTHOR),
				get_section_str(header, MODULE_SECTION_NAME),
				get_section_str(header, MODULE_SECTION_DESCRIPTION));
	}
}

SignalHandlerFn module_cur_handler = NULL;

void module_init(struct stivale2_struct *s)
{
	uint64_t i;
	struct stivale2_struct_tag_memmap *memory_map;
	memory_map = stivale2_get_tag(s, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	for(i = 0; i < memory_map->entries; ++i)
	{
		ModuleHeader *header;
		struct stivale2_mmap_entry *cur = &memory_map->memmap[i];
		if(cur->type != STIVALE2_MMAP_KERNEL_AND_MODULES)
		{
			continue;
		}

		header = (ModuleHeader *)cur->base;
		if(header->Magic != MODULE_MAGIC)
		{
			continue;
		}

		if(header->Type == MODULE_TYPE_EXECUTABLE)
		{
			uint64_t fn_addr = (uint64_t)header +
				get_section(header, MODULE_SECTION_SIGNAL_HANDLER)->Start;
			ModuleSectionSignalHandler *fn =
				(ModuleSectionSignalHandler *)&fn_addr;

			ModuleInit init_data =
			{
				functions
			};

			if(!strcmp(get_section_str(header, MODULE_SECTION_NAME), "Shell"))
			{
				module_cur_handler = fn->SignalHandler;
			}

			fn->SignalHandler(SIGNAL_ID_INIT, &init_data);
		}
	}
}
