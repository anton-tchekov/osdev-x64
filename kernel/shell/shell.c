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

void shell_enter(void)
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
