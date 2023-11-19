#include "shell.h"
#include "keyboard.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <shutdown.h>

#define SHELL_BUFFER_SIZE 256

typedef struct
{
    int Cursor;
    int Line;
    char Buffer[SHELL_BUFFER_SIZE];
} Shell;



static Shell shell;

static void shell_handle_command (char* cmd);


static void event_key (int key, int ascii, int released) {
    if(released)
    {
        return;
    }

    if(isprint(ascii))
    {
        shell.Buffer[shell.Cursor++] = ascii;
        printk("%c", ascii);
    } else if(ascii == '\n') {
        shell_handle_enter();
    } else if(ascii == '\b' && shell.Cursor > 0) {
        shell.Buffer[--shell.Cursor] = 0;
        printk("\b");
    }
}


void shell_init(){
    shell.Cursor = 0;
    shell.Line = 0;

    keyboard_event_register(event_key);

    printk("\n\nImaginaryOS (iOS) made by Anton and Tim \n\n"
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

    shell_handle_enter();
}

void shell_handle_enter () {
    printk("\n");
    shell_handle_command(shell.Buffer);
    printk("\nImaginaryOS> ");
    shell_clear_buffer();
}

static void shell_handle_command (char* cmd) {
    char *argv[16];
    char *p = cmd;
    int argc = 0;

    while(*p)
    {
        while(isspace(*p)) {
            ++p;
        }

        argv[argc++] = p;
        while(*p && !isspace(*p)) {
            ++p;
        }

        *p++ = '\0';
    }

    printk("argc = %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printk("%d. %s\n", i, argv[i]);
    }

    if(strcmp(argv[0], "shutdown") == 0)
    {
        shutdown();
    }
}

void shell_clear_buffer () {
    for (int i = 0; i < SHELL_BUFFER_SIZE; ++i) {
        shell.Buffer[i] = 0;
    }
    shell.Cursor = 0;
}

void puts(const char* line, int x, int y, int c) {
    printk(line);
}