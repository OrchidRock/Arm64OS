//
// Created by mimose on 25-3-23.
//

#ifndef ASM_PROCESSOR_H
#define ASM_PROCESSOR_H

struct cpu_context {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp;
    unsigned long sp;
    unsigned long pc;
};


#endif //ASM_PROCESSOR_H
