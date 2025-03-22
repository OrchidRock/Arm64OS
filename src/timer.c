#include "asm/arm_local_regs.h"
#include "asm/raw_irq_regs.h"
#include "asm/timer_regs.h"
#include "timer.h"
#include "io.h"
#include "arm-gic.h"

#define HZ             (25)
#define NSEC_PER_SEC   (1000000000L)
static unsigned int PNS_VAL = NSEC_PER_SEC / HZ;

static unsigned int sys_timer_val = 0;
static unsigned int sys_val = 200000;

static void generic_timer_init(void)
{
    /* enable timer. */
    asm volatile (
        "mov x0, #1\n"
        "msr cntp_ctl_el0, x0"
        :
        :
        : "memory");
}

static int generic_timer_reset(unsigned int val)
{
    asm volatile (
        "msr cntp_tval_el0, %x[timer_val]"
        :
        : [timer_val] "r" (val)
        : "memory");
    return 0;
}

static inline void enable_timer_interrupt(void)
{
    writel(CNT_PNS_IRQ, TIMER_CNTRL0);
}

void timer_init(void)
{
    generic_timer_init();
    generic_timer_reset(PNS_VAL);

    gicv2_unmask_irq(GENERIC_TIMER_IRQ);

    enable_timer_interrupt();
}

void handle_timer_irq(void)
{
    generic_timer_reset(PNS_VAL);
    printk("Core 0 Timer interrupt received\r\n");
}


void system_timer_init(void)
{
    sys_timer_val = readl(TIMER_CLO);
    sys_timer_val += sys_val;
    writel(sys_timer_val, TIMER_C1);

    gicv2_unmask_irq(SYSTEM_TIMER1_IRQ);

    /* enable system timer*/
    writel(SYSTEM_TIMER_IRQ_1, ENABLE_IRQS_0);
}

void handle_stimer_irq(void)
{
    sys_timer_val += sys_val;
    writel(sys_timer_val, TIMER_C1);
    writel(TIMER_CS_M1, TIMER_CS);
    printk("Sytem Timer1 interrupt \n");
}