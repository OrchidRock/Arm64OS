#include "io.h"
#include "timer.h"
#include "asm/arm_local_regs.h"

void irq_handler(void)
{
    unsigned int irq = readl(ARM_LOCAL_IRQ_SOURCE0);
    switch (irq) {
    case (CNT_PNS_IRQ):
        handle_timer_irq();
        break;
    default:
        printk("Unknown pending IRQ %x\r\n", irq);
    }
}
