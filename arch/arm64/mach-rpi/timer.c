#include "mach/arm_local_regs.h"
#include "asm/irq.h"
#include "mach/timer.h"
#include "mach/irq.h"
#include "timer.h"
#include "asm/io.h"
#include "arm-gic.h"
#include "printk.h"
#include "sched.h"
#include "irq.h"

static unsigned int arch_timer_rate;

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

static unsigned int generic_timer_get_freq(void)
{
	unsigned int freq;

	asm volatile(
		"mrs %0, cntfrq_el0"
		: "=r" (freq)
		:
		: "memory");

	return freq;
}


static inline void enable_timer_interrupt(void)
{
    writel(CNT_PNS_IRQ, TIMER_CNTRL0);
}

int handle_timer_irq(int irq, void *param)
{
    //printk("Core 0 Timer interrupt received\r\n");
    generic_timer_reset(arch_timer_rate);
    tick_handle_periodic();

    return 0;
}

void timer_init(void)
{
  	arch_timer_rate = generic_timer_get_freq();
	printk("cntp freq:0x%x\r\n", arch_timer_rate);
	arch_timer_rate /= HZ;

    generic_timer_init();
    generic_timer_reset(arch_timer_rate);

    request_irq(GENERIC_TIMER_IRQ, handle_timer_irq, "core timer", NULL);
    enable_timer_interrupt();
}

void system_timer_init(void)
{
//    sys_timer_val = readl(TIMER_CLO);
//    sys_timer_val += sys_val;
//    writel(sys_timer_val, TIMER_C1);
//
//    gicv2_unmask_irq(SYSTEM_TIMER1_IRQ);
//
//    /* enable system timer*/
//    writel(SYSTEM_TIMER_IRQ_1, ENABLE_IRQS_0);
}

void handle_stimer_irq(void)
{
//    sys_timer_val += sys_val;
//    writel(sys_timer_val, TIMER_C1);
//    writel(TIMER_CS_M1, TIMER_CS);
//    printk("Sytem Timer1 interrupt \n");
}