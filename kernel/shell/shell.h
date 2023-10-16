#ifndef __SHELL_H__
#define __SHELL_H__

void shell_init(void);

void shell_handle_enter (void);

void shell_clear_buffer ();

void event_key (int key, int ascii);

void puts (const char* line, int x, int y, int c);

#endif
