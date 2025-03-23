//
// Created by mimose on 25-3-23.
//

#include "sched.h"

void irq_handler(void)
{
    __irq_enter();
    arch_irq_handler();
    __irq_exit();
}