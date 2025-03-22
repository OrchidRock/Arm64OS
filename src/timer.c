#include "asm/arm_local_regs.h"
#include "timer.h"
#include "io.h"

#define HZ             (250)
#define NSEC_PER_SEC   (1000000000L)
static unsigned int PNS_VAL = NSEC_PER_SEC / HZ;

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

static void enable_timer_interrupt(void)
{
    writel(CNT_PNS_IRQ, TIMER_CNTRL0);
}

void timer_init(void)
{
    generic_timer_init();
    generic_timer_reset(PNS_VAL);
    enable_timer_interrupt();
}

void handle_timer_irq(void)
{
    generic_timer_reset(PNS_VAL);
    printk("Core 0 Timer interrupt received\r\n");
}
