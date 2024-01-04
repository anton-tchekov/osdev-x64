#ifndef __MODFUNC_H__
#define __MODFUNC_H__

typedef void (*printf_fn)(const char *s, ...);

#define printf(...) ((printf_fn)fns[KERNEL_FN_PRINTF])(__VA_ARGS__)

#endif
