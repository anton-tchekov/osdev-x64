#include <stddef.h>
#include "stivale2.h"

static uint8_t stack[4096];

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag =
{
	{
		STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, 0
	},
	0, 0, 0
};

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr =
{
	0,
	(uintptr_t)stack + sizeof(stack),
	(1 << 1) | (1 << 2),
	(uintptr_t)&framebuffer_hdr_tag
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
