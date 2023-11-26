#include "shell.h"
#include "ps2.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

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
        printf("%c", ascii);
    } else if(ascii == '\n') {
        shell_handle_enter();
    } else if(ascii == '\b' && shell.Cursor > 0) {
        shell.Buffer[--shell.Cursor] = 0;
        printf("\b");
    }
    (void)key;
}


void shell_init(){
    shell.Cursor = 0;
    shell.Line = 0;

    keyboard_event_register(event_key);

    printf("\n\nImaginaryOS (iOS) made by Anton and Tim \n\n"
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
    printf("\n");
    shell_handle_command(shell.Buffer);
    printf("\nImaginaryOS> ");
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

    printf("argc = %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printf("%d. %s\n", i, argv[i]);
    }
}

void shell_clear_buffer () {
    for (int i = 0; i < SHELL_BUFFER_SIZE; ++i) {
        shell.Buffer[i] = 0;
    }
    shell.Cursor = 0;
}