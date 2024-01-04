#ifndef __MODFUNC_H__
#define __MODFUNC_H__

typedef void (*printf_fn)(const char *s, ...);
typedef int (*isprint_fn)(int);
typedef int (*terminal_set_color_fn)(int, int);
typedef void (*keyboard_event_register_fn)(void (*)(int, int, int));

#define printf(...) ((printf_fn)fns[KERNEL_FN_PRINTF])(__VA_ARGS__)
#define isprint(...) ((isprint_fn)fns[KERNEL_FN_ISPRINT])(__VA_ARGS__)
#define terminal_set_color(...) ((terminal_set_color_fn)fns[KERNEL_FN_TERMINAL_SET_COLOR])(__VA_ARGS__)
#define keyboard_event_register(...) ((keyboard_event_register_fn)fns[KERNEL_FN_KEYBOARD_REGISTER_FN])(__VA_ARGS__)

#endif
