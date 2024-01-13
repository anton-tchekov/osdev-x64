#ifndef __MODULE_H__
#define __MODULE_H__

#include "stivale2.h"
#include <stdint.h>

#define MODULE_MAGIC 0x47545441

typedef enum
{
	MODULE_TYPE_RAW,
	MODULE_TYPE_EXECUTABLE,
	MODULE_TYPE_TEXT
} ModuleType;

typedef enum
{
	MODULE_ID_TEMPLATE
} ModuleId;

typedef enum
{
	SIGNAL_ID_INIT,
	SIGNAL_ID_KEYBOARD,
	SIGNAL_ID_TIMER
} SignalId;

typedef enum
{
	MODULE_SECTION_NAME,
	MODULE_SECTION_AUTHOR,
	MODULE_SECTION_DESCRIPTION,
	MODULE_SECTION_DEPENDENCIES,
	MODULE_SECTION_DATA,
	MODULE_SECTION_SIGNAL_HANDLER
} ModuleSectionType;

typedef struct
{
	uint32_t Type;
	uint64_t Start;
} ModuleSection;

typedef struct
{
	uint32_t Magic;
	uint32_t Type;
	uint32_t Id;
	uint32_t NumSections;
	ModuleSection Sections[];
} ModuleHeader;

typedef enum
{
	KERNEL_FN_PRINTF,
	KERNEL_FN_TERMINAL_SET_COLOR,
	KERNEL_FN_GRAPHICS_RECT,
} ModuleFunctions;

typedef struct
{
	const uint64_t *Functions;
} ModuleInit;

typedef struct
{
	uint32_t Key, Codepoint, Released;
} ModuleKeyEvent;

typedef void (*SignalHandlerFn)(uint32_t, void *);

typedef struct
{
	SignalHandlerFn SignalHandler;
} ModuleSectionSignalHandler;

void module_list(struct stivale2_struct *s);
void module_init(struct stivale2_struct *s);

#endif
