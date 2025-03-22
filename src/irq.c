#include "io.h"
#include "timer.h"
#include "asm/arm_local_regs.h"

#ifdef ENABLE_GIC_V2
#include "arm-gic.h"
#endif

void irq_handler(void)
{
#ifndef ENABLE_GIC_V2
    unsigned int irq = readl(ARM_LOCAL_IRQ_SOURCE0);
    switch (irq) {
    case (CNT_PNS_IRQ):
        handle_timer_irq();
        break;
    default:
        printk("Unknown pending IRQ %x\r\n", irq);
    }
#else
    gic_handle_irq();
#endif
}
