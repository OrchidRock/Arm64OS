#include "asm/io.h"
#include "timer.h"
#include "mach/arm_local_regs.h"

#ifdef ENABLE_GIC_V2
#include "arm-gic.h"
#endif

void arch_irq_init(void)
{
#ifndef ENABLE_GIC_V2
#else
    gic_init(0, GIC_V2_DISTRIBUTOR_BASE, GIC_V2_CPU_INTERFACE_BASE);
#endif
}
