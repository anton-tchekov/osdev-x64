#include "module.h"
#include "modfunc.h"
#include <stddef.h>

printf_fn printf = NULL;

void load_functions(void* data)
{
    ModuleInit *mi = (ModuleInit *)data;
    uint64_t *fns = mi->Functions;

    printf = (printf_fn)fns[KERNEL_FN_PRINTF];
}
