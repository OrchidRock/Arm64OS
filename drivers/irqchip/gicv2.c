#include "arm-gic.h"
#include "asm/io.h"
#include "mach/irq.h"
#include "timer.h"
#include "printk.h"
#include "irq.h"

struct gic_chip_data_t {
    unsigned long raw_dist_base;
    unsigned long raw_cpu_base;
    struct irq_domain *domain;
    struct irq_chip *chip;
    unsigned int gic_irqs;
};

#define gic_dist_base(d) ((d)->raw_dist_base)
#define gic_cpu_base(d) ((d)->raw_cpu_base)

#define ARM_GIC_MAX_NR 1
static struct gic_chip_data_t gic_data[ARM_GIC_MAX_NR];

/* IRQs start ID */
#define HW_IRQ_START 16

static inline unsigned int gic_irq(struct irq_desc *d)
{
    return d->hwirq;
}

static unsigned long gic_get_cpu_base(void)
{
    struct gic_chip_data_t *gic = &gic_data[0];
    return gic_cpu_base(gic);
}

static unsigned long gic_get_dist_base(void)
{
    struct gic_chip_data_t *gic = &gic_data[0];
    return gic_dist_base(gic);
}

static unsigned int gic_get_cpumask(struct gic_chip_data_t *gic)
{
    unsigned long base = gic_dist_base(gic);
    unsigned int mask, i;

    for (i = mask = 0; i < 32; i += 4) {
        mask = readl(base + GIC_DIST_TARGET + i);
        mask |= mask >> 16;
        mask |= mask >> 8;
        if (mask)
            break;
    }

    return mask;
}

static void gic_set_irq(struct irq_desc *d, unsigned int offset)
{
    unsigned int mask = 1 << (gic_irq(d) % 32);
    writel(mask, gic_get_dist_base() + offset + (gic_irq(d) / 32) * 4);
}

static void gicv2_eoi_irq(struct irq_desc *d)
{
    writel(gic_irq(d), gic_get_cpu_base() + GIC_CPU_EOI);
}

static void gicv2_mask_irq(struct irq_desc *d)
{
    gic_set_irq(d, GIC_DIST_ENABLE_CLEAR);
}

static void gicv2_unmask_irq(struct irq_desc *d)
{
    gic_set_irq(d, GIC_DIST_ENABLE_SET);
}

static struct irq_chip gicv2_chip = {
    .name = "arm gicv2",
    .irq_mask = gicv2_mask_irq,
    .irq_unmask = gicv2_unmask_irq,
    .irq_eoi = gicv2_eoi_irq,
};

static int gicv2_irq_map(struct irq_domain *d,
        unsigned int virq, unsigned int hwirq)
{
    /* handle the SGIs */
    if (hwirq >= 16)
        irq_domain_set_hwirq_chip(d, virq, hwirq, &gicv2_chip, d->host_data);

    return 0;
}

static const struct irq_domain_ops gicv2_irq_domain_ops = {
    .map = gicv2_irq_map,
};

static void gic_dist_init(struct gic_chip_data_t *gic)
{
    unsigned long base = gic_dist_base(gic);
    unsigned int cpumask;
    unsigned int gic_irqs = gic->gic_irqs;
    int i;

    /* disable interrupt. */
    writel(GICD_DISABLE, base + GIC_DIST_CTRL);

    /* set interrupt route: SPI (irq32 - irqxx) */
    cpumask = gic_get_cpumask(gic);
    cpumask |= cpumask << 8;
    cpumask |= cpumask << 16;

    for (i = 32; i < gic_irqs; i += 4)
        writel(cpumask, base + GIC_DIST_TARGET + i * 4 / 4);

    /* Set all global interrupts to be level triggered, active low */
    for (i = 32; i < gic_irqs; i += 16)
        writel(GICD_INT_ACTLOW_LVLTRIG, base + GIC_DIST_CONFIG + i / 4);

    /* Deactivate and disable all irqs (SGI, PPI, SPI). */
    for (i = 0; i < gic_irqs; i += 32) {
        writel(GICD_INT_EN_CLR_X32, base + GIC_DIST_ACTIVE_CLEAR + i / 8);
        writel(GICD_INT_EN_CLR_X32, base + GIC_DIST_ENABLE_CLEAR + i / 8);
    }

    /* Activate SGI.  */
    writel(GICD_INT_EN_SET_SGI, base + GIC_DIST_ENABLE_SET);

    /* Anable group 0 and group1 interrupt forwarding. */
    writel(GICD_ENABLE, base + GIC_DIST_CTRL);
}

static void gic_handle_irq(void)
{
    struct gic_chip_data_t *gic = &gic_data[0];
    unsigned long base = gic_cpu_base(gic);
    unsigned int irqstat, irqnr;

    do {
        irqstat = readl(base + GIC_CPU_INTACK);
        irqnr = irqstat & GICC_IAR_INT_ID_MASK;

//        if (irqnr == GENERIC_TIMER_IRQ)
//            handle_timer_irq();
//        else if (irqnr == SYSTEM_TIMER1_IRQ)
//            handle_stimer_irq();

//        gicv2_eoi_irq(irqnr);
        if (irqnr > 15 && irqnr < 1021)
            handle_domain_irq(gic->domain, irqnr);

    } while (0);
}

void gic_cpu_init(struct gic_chip_data_t *gic)
{
    int i;
    unsigned long base = gic_cpu_base(gic);
    unsigned long dist_base = gic_dist_base(gic);

    /* Set priority on PPI and SGI interrupts. */
    for (i = 0; i < 32; i += 4)
        writel(0xA0A0A0A0, dist_base + GIC_DIST_PRI + i * 4 / 4);

    writel(GICC_INT_PRI_THRESHOLD, base + GIC_CPU_PRIMASK);
    writel(GICC_ENABLE, base + GIC_CPU_CTRL);
}

int gic_init(int chip, unsigned long dist_base, unsigned long cpu_base)
{
    struct gic_chip_data_t *gic;
    int gic_irqs;
    int virq_base;

    gic = &gic_data[chip];
    gic->raw_dist_base = dist_base;
    gic->raw_cpu_base = cpu_base;
    gic->chip = &gicv2_chip;

    gic_irqs = readl(gic_dist_base(gic) + GIC_DIST_CTR) & 0x1F;
    gic_irqs = (gic_irqs + 1) * 32;
    if (gic_irqs > 1020)
        gic_irqs = 1020;
    gic->gic_irqs = gic_irqs;

    /* allocate irqdesc, skip over SGIs and PPIs */
    gic_irqs -= HW_IRQ_START;
    virq_base = irq_alloc_descs(HW_IRQ_START, gic_irqs);
    gic->domain = irq_domain_add(virq_base, HW_IRQ_START, gic_irqs, &gicv2_irq_domain_ops, gic);
    if (!gic->domain)
        return -1;

    printk("%s: cpu_base:0x%x, dist_base:0x%x, gic_irqs:%d\n",
            __func__, cpu_base, dist_base, gic->gic_irqs);

    gic_dist_init(gic);
    gic_cpu_init(gic);

    if (chip == 0)
        set_handle_irq(gic_handle_irq);

    return 0;
}