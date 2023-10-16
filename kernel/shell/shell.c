#include "shell.h"
#include <ctype.h>
#include <stdio.h>

#define SHELL_BUFFER_SIZE 256

typedef struct
{
    int Cursor;
    int Line;
    char Buffer[SHELL_BUFFER_SIZE];
} Shell;



static Shell shell;

static void shell_handle_command (char* cmd);

void shell_init(){
    shell.Cursor = 0;
    shell.Line = 0;

    activate_keyboard_processing(event_key);

    printk(GFX_RED, "\n\nImaginaryOS (iOS) made by Anton and Tim \n\n"
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

void event_key (int key, int ascii) {
    if(isprint(ascii))
    {
        shell.Buffer[shell.Cursor++] = ascii;
        printk(GFX_RED, "%c", ascii);
    } else if(ascii == '\r') {
        shell_handle_enter();
    } else if(ascii == '\b' && shell.Cursor > 0) {
        shell.Buffer[--shell.Cursor] = 0;
        printk(GFX_RED, "\b");
    }
}

void shell_handle_enter () {
    printk(GFX_WHITE, "\n");
    shell_handle_command(shell.Buffer);
    printk(GFX_GREEN, "\nImaginaryOS> ");
    shell_clear_buffer();
}

static void shell_handle_command (char* cmd) {
    char *argv[16];
    char *p = cmd;
    int argc = 0;

    printk(GFX_WHITE, "here!\n");
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

    printk(GFX_WHITE, "argc = %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printk(GFX_WHITE, "%d. %s\n", i, argv[i]);
    }
}

void shell_clear_buffer () {
    for (int i = 0; i < SHELL_BUFFER_SIZE; ++i) {
        shell.Buffer[i] = 0;
    }
    shell.Cursor = 0;
}

void puts(const char* line, int x, int y, int c) {
    printk(GFX_RED, line);
}