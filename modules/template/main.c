#include "../../kernel/module.h"
#include "../modfunc.h"

const uint64_t *fns;

static const char name[] = "Template Module";
static const char author[] = "Anton Tchekov";
static const char desc[] =
	"Basic executable module template that does essentially nothing";

static void mmain()
{
	printf("Hello from Template Module\n");
}

static void signal_handler(uint32_t signal_id, void *data)
{
	switch(signal_id)
	{
	case SIGNAL_ID_INIT:
		fns = ((ModuleInit *)data)->Functions;
		mmain();
		break;
	}
}

static const ModuleHeader module_header
	__attribute__((section(".header")))
	__attribute__((__used__)) =
{
	.Magic = MODULE_MAGIC,
	.Type = MODULE_TYPE_EXECUTABLE,
	.Id = MODULE_ID_TEMPLATE,
	.NumSections = 4,
	.Sections =
	{
		{
			.Type = MODULE_SECTION_NAME,
			.Start = (uintptr_t)name
		},
		{
			.Type = MODULE_SECTION_AUTHOR,
			.Start = (uintptr_t)author
		},
		{
			.Type = MODULE_SECTION_DESCRIPTION,
			.Start = (uintptr_t)desc
		},
		{
			.Type = MODULE_SECTION_SIGNAL_HANDLER,
			.Start = (uintptr_t)signal_handler
		},
	}
};
