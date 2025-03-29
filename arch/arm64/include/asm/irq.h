//
// Created by mimose on 25-3-23.
//

#ifndef ASM_IRQ_H
#define ASM_IRQ_H

static inline void arch_local_irq_disable(void)
{
    asm volatile(
        "msr    daifset, #2"
        :
        :
        : "memory");
}

static inline void arch_local_irq_enable(void)
{
    asm volatile(
        "msr    daifclr, #2"
        :
        :
        : "memory");
}

void arch_irq_init(void);

#endif //ASM_IRQ_H
