#include <stdint.h>
#include "stivale2.h"
#include "keyboard.h"
#include "shell.h"
#include "stdio.h"
#include "ssfn.h"

static int shell_prompt_x_barrier;
static int shell_prompt_y_barrier;

void shell_screen_init(void)
{
	framebuffer_reset_screen();
	shell_prompt();
	activate_keyboard_processing(shell_print_char);
}

void shell_prompt(void)
{
	printk(GFX_PURPLE, "> ");
	shell_prompt_x_barrier = ssfn_dst.x;
	shell_prompt_y_barrier = ssfn_dst.y;
}

void shell_print_char(KEY_INFO_t key_info)
{
	if(key_info.ascii_character == '\0')
	{
		return;
	}
	else if(key_info.ascii_character == KEY_RETURN)
	{
		if(ssfn_dst.y + gfx.glyph_height == gfx.fb_height)
		{
			shell_prompt_y_barrier += gfx.glyph_height;
		}

		printk(GFX_BLUE, "\n");
		shell_prompt();
	}
	else if (key_info.ascii_character == KEY_BACKSPACE)
	{
		if (ssfn_dst.x == shell_prompt_x_barrier && ssfn_dst.y == shell_prompt_y_barrier)
			return;

		printk(GFX_BLUE, "\b");
	}
	else
	{

		if (ssfn_dst.y + gfx.glyph_height == gfx.fb_height && ssfn_dst.x + gfx.glyph_width == gfx.fb_width)
			shell_prompt_y_barrier -= gfx.glyph_height;

		printk(GFX_BLUE, "%c", key_info.ascii_character);
	}
}
