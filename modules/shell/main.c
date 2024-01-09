#include "../../kernel/module.h"
#include "../../kernel/terminal.h"
#include "../modfunc.h"

const uint64_t *fns;

static const char name[] = "Shell";
static const char author[] = "Tim Gabrikowski";
static const char desc[] = "Shell module";

#define SHELL_BUFFER_SIZE 256

typedef struct
{
	int Cursor;
	int Line;
	char Buffer[SHELL_BUFFER_SIZE];
} Shell;

static Shell shell;

static void shell_command(char *cmd)
{
	char *argv[16];
	char *p;
	int argc, i;

	argc = 0;
	p = cmd;
	i = 1;
	while(*p)
	{
		if(*p == ' ')
		{
			*p = '\0';
			i = 1;
		}
		else
		{
			if(i)
			{
				argv[argc++] = p;
				i = 0;
			}
		}

		++p;
	}

	printf("argc = %d\n", argc);
	for(i = 0; i < argc; ++i)
	{
		printf("%d. %s\n", i, argv[i]);
	}
}

static void shell_prompt(void)
{
	shell.Cursor = 0;
	terminal_set_color(TERMINAL_GREEN, TERMINAL_BLACK);
	printf("\nImaginaryOS>");
	terminal_set_color(TERMINAL_WHITE, TERMINAL_BLACK);
	printf(" ");
}

static void shell_enter(void)
{
	printf("\n");
	shell_command(shell.Buffer);
	shell_prompt();
}

static void event_key(int key, int ascii, int released)
{
	if(released)
	{
		return;
	}

	if(isprint(ascii))
	{
		shell.Buffer[shell.Cursor++] = ascii;
		printf("%c", ascii);
	}
	else if(ascii == '\n')
	{
		shell.Buffer[shell.Cursor] = '\0';
		shell_enter();
	}
	else if(ascii == '\b' && shell.Cursor > 0)
	{
		shell.Buffer[--shell.Cursor] = 0;
		printf("\b");
	}

	(void)key;
}

static void shell_init(void)
{
	shell.Cursor = 0;
	shell.Line = 0;
	keyboard_event_register(event_key);
	terminal_set_color(TERMINAL_RED, TERMINAL_BLACK);
	printf("\n\nImaginaryOS made by Anton and Tim\n\n"
		"###########    ###########\n"
		"###########    ###########\n"
		"###                    ###\n"
		"###                    ###\n"
		"###                    ###\n"
		"\n"
		"###                    ###\n"
		"###                    ###\n"
		"###                    ###\n"
		"###########    ###########\n"
		"###########    ###########\n\n");

	shell_prompt();
}

static void mmain(void)
{
	shell_init();
}

static void signal_handler(int signal_id, void *data)
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
