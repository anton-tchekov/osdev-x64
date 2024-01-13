#ifndef __MODFUNC_H__
#define __MODFUNC_H__

#include <stddef.h>

typedef void (*printf_fn)(const char *s, ...);
typedef void (*terminal_set_color_fn)(uint32_t, uint32_t);
typedef void (*graphics_rect_fn)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

#define printf(...) ((printf_fn)fns[KERNEL_FN_PRINTF])(__VA_ARGS__)
#define terminal_set_color(...) ((terminal_set_color_fn)fns[KERNEL_FN_TERMINAL_SET_COLOR])(__VA_ARGS__)
#define graphics_rect(...) ((graphics_rect_fn)fns[KERNEL_FN_GRAPHICS_RECT])(__VA_ARGS__)

#endif
