#include <stddef.h>
#include "stivale2.h"

static uint8_t stack[4096];

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag =
{
	.tag =
	{
		.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
		.next = 0
	},
	.framebuffer_width = 0,
	.framebuffer_height = 0,
	.framebuffer_bpp = 0
};

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr =
{
	.entry_point = 0,
	.stack = (uintptr_t)stack + sizeof(stack),
	.flags = (1 << 1) | (1 << 2),
	.tags = (uintptr_t)&framebuffer_hdr_tag
};

void *stivale2_get_tag(struct stivale2_struct *s, uint64_t id)
{
	struct stivale2_tag *current_tag = (struct stivale2_tag *)s->tags;
	while(current_tag)
	{
		if(current_tag->identifier == id)
		{
			return current_tag;
		}

		current_tag = (struct stivale2_tag *)current_tag->next;
	}

	return NULL;
}
