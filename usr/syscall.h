//
// Created by mimose on 25-3-29.
//

#ifndef USR_SYSCALL_H
#define USR_SYSCALL_H


extern int __clone(int (*fn)(void *arg), void *child_stack,
        int flags, void *arg);
extern unsigned long syscall(int nr, ...);

int clone(int (*fn)(void *arg), void *child_stack,
        int flags, void *arg);
unsigned long my_malloc(void);

#endif //SYSCALL_H
