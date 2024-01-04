#include "module.h"
#include <stdio.h>
#include <inttypes.h>
#include "../modules/module.h"

typedef void (*SignalHandlerFn)(int, void *);

static const uint64_t functions[] = {
        (uint64_t) printf
};

static ModuleSection* getSection(ModuleHeader* header, ModuleSectionType type) {
    for (int i = 0; i < header->NumSections; ++i) {
        if(header->Sections[i].Type == type) {
            return &header->Sections[i];
        }
    }
    return NULL;
}

static char *getSectionString(ModuleHeader* header, ModuleSectionType type) {
    return (char *)header + getSection(header, type)->Start;
}

void module_init(struct stivale2_struct *s)
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
                header->NumSections, cur->base,
                cur->length);

        printf("Author: %s\nName: %s\nDescription: %s\n",
               getSectionString(header, MODULE_SECTION_AUTHOR),
               getSectionString(header, MODULE_SECTION_NAME),
               getSectionString(header, MODULE_SECTION_DESCRIPTION));

        if(header->Type == MODULE_TYPE_EXECUTABLE)
        {
            ModuleSection *sect = getSection(header, MODULE_SECTION_SIGNAL_HANDLER);
            SignalHandlerFn fn = (SignalHandlerFn)((char *)header + sect->Start);
            ModuleInit init_data = {
                .Functions = functions,
            };
            fn(SIGNAL_ID_INIT, &init_data);
        }
    }
}
