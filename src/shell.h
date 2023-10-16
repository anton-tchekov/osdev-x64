#ifndef SHELL_SCREEN_H
#define SHELL_SCREEN_H

extern struct GFX_Struct gfx;

void shell_screen_init(void);
void shell_prompt(void);
void shell_print_char(KEY_INFO_t key_info);

#endif
