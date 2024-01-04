#include "shell.h"
#include "ps2.h"
#include <ctype.h>
#include <stdio.h>
#include "terminal.h"

#define SHELL_BUFFER_SIZE 256

typedef struct
{
	int Cursor;
	int Line;
	char Buffer[SHELL_BUFFER_SIZE];
} Shell;

static Shell shell;

static void shell_handle_command(char *cmd)
{
	char *argv[16];
	char *p = cmd;
	int argc = 0;
	int i;

	while(*p)
	{
		while(isspace(*p))
		{
			++p;
		}

		argv[argc++] = p;
		while(*p && !isspace(*p))
		{
			++p;
		}

		*p++ = '\0';
	}

	printf("argc = %d\n", argc);
	for(i = 0; i < argc; ++i)
	{
		printf("%d. %s\n", i, argv[i]);
	}
}

static void shell_clear_buffer(void)
{
	shell.Buffer[0] = '\0';
	shell.Cursor = 0;
}

static void shell_prompt(void)
{
	terminal_set_color(TERMINAL_GREEN, TERMINAL_BLACK);
	printf("\nImaginaryOS> ");
	shell_clear_buffer();
	terminal_set_color(TERMINAL_WHITE, TERMINAL_BLACK);
}

void shell_enter(void)
{
	printf("\n");
	shell_handle_command(shell.Buffer);
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
		shell_enter();
	}
	else if(ascii == '\b' && shell.Cursor > 0)
	{
		shell.Buffer[--shell.Cursor] = 0;
		printf("\b");
	}

	(void)key;
}

void shell_init(void)
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
