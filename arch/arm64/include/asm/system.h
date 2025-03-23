#ifndef PTREGS_H
#define PTREGS_H

/* save interrupt context or exception context.
 * contents:
 *     x0 - x30  general registers
 *     sp
 *     pc
 *     pstate
 */

struct pt_regs {
    unsigned long regs[31];
    unsigned long sp;
    unsigned long pc;
    unsigned long pstate;
};

#endif //PTREGS_H
