//
// Created by tim on 11/26/23.
//

#ifndef OSDEV_X64_MODFUNC_H
#define OSDEV_X64_MODFUNC_H

typedef void (*printf_fn)(const char *s, ...);

extern printf_fn printf;

#endif //OSDEV_X64_MODFUNC_H
