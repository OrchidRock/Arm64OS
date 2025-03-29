//
// Created by mimose on 25-3-29.
//

#include "uapi/syscall.h"
#include "usr/syscall.h"

int clone(int (*fn)(void *arg), void *child_stack,
        int flags, void *arg)
{

    return __clone(fn, child_stack, flags, arg);
}

unsigned long my_malloc(void)
{
    return syscall(__NR_malloc);
}
