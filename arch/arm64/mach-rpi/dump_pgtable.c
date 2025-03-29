#include "asm/pgtable_types.h"
#include "asm/pgtable_prot.h"
#include "asm/pgtable.h"
#include "mach/base.h"
#include "asm/mm.h"
#include "printk.h"

typedef unsigned int size_t;

extern char idmap_pgd_dir[];
extern char readonly_page[];

struct addr_marker {
    unsigned long start_address;
    char *name;
};

struct prot_bits {
    unsigned long mask;
    unsigned long val;
    const char *set;
    const char *clear;
};

struct pg_level {
    const struct prot_bits *bits;
    const char *name;
    size_t num;
    unsigned long mask;
};

static const struct prot_bits pte_bits[] = {
    {
        .mask	= PTE_VALID,
        .val	= PTE_VALID,
        .set	= " ",
        .clear	= "F",
    }, {
        .mask	= PTE_USER,
        .val	= PTE_USER,
        .set	= "USR",
        .clear	= "   ",
    }, {
        .mask	= PTE_RDONLY,
        .val	= PTE_RDONLY,
        .set	= "ro",
        .clear	= "RW",
    }, {
        .mask	= PTE_PXN,
        .val	= PTE_PXN,
        .set	= "NX",
        .clear	= "x ",
    }, {
        .mask	= PTE_SHARED,
        .val	= PTE_SHARED,
        .set	= "SHD",
        .clear	= "   ",
    }, {
        .mask	= PTE_AF,
        .val	= PTE_AF,
        .set	= "AF",
        .clear	= "  ",
    }, {
        .mask	= PTE_NG,
        .val	= PTE_NG,
        .set	= "NG",
        .clear	= "  ",
    }, {
        .mask	= PTE_CONT,
        .val	= PTE_CONT,
        .set	= "CON",
        .clear	= "   ",
    }, {
        .mask	= PTE_TABLE_BIT,
        .val	= PTE_TABLE_BIT,
        .set	= "   ",
        .clear	= "BLK",
    }, {
        .mask	= PTE_UXN,
        .val	= PTE_UXN,
        .set	= "UXN",
    }, {
        .mask	= PTE_ATTRINDX_MASK,
        .val	= PTE_ATTRINDX(MT_DEVICE_nGnRnE),
        .set	= "DEVICE/nGnRnE",
    }, {
        .mask	= PTE_ATTRINDX_MASK,
        .val	= PTE_ATTRINDX(MT_DEVICE_nGnRE),
        .set	= "DEVICE/nGnRE",
    }, {
        .mask	= PTE_ATTRINDX_MASK,
        .val	= PTE_ATTRINDX(MT_DEVICE_GRE),
        .set	= "DEVICE/GRE",
    }, {
        .mask	= PTE_ATTRINDX_MASK,
        .val	= PTE_ATTRINDX(MT_NORMAL_NC),
        .set	= "MEM/NORMAL-NC",
    }, {
        .mask	= PTE_ATTRINDX_MASK,
        .val	= PTE_ATTRINDX(MT_NORMAL),
        .set	= "MEM/NORMAL",
    }
};

static struct pg_level pg_level[] = {
    {
    },
    { /* pgd */
        .name	= "PGD",
        .bits	= pte_bits,
        .num	= ARRAY_SIZE(pte_bits),
    },
    { /* pud */
        .name	= "PUD",
        .bits	= pte_bits,
        .num	= ARRAY_SIZE(pte_bits),
    },
    { /* pmd */
        .name	= "PMD",
        .bits	= pte_bits,
        .num	= ARRAY_SIZE(pte_bits),
    },
    { /* pte */
        .name	= "PTE",
        .bits	= pte_bits,
        .num	= ARRAY_SIZE(pte_bits),
    },
};

static const struct addr_marker address_markers[] = {
    { 0, "Identical mapping" },
};

static void dump_prot(unsigned long prot, const struct prot_bits *bits, size_t num)
{
    unsigned i;

    for (i = 0; i < num; i++, bits++) {
        const char *s;

        if ((prot & bits->mask) == bits->val)
            s = bits->set;
        else
            s = bits->clear;

        if (s)
            printk(" %s", s);
    }
}

static int print_pgtable(unsigned long start, unsigned long end,
        int level, unsigned long val)
{
    static const char units[] = "KMGT";
    unsigned long prot = val & pg_level[level].mask;
    unsigned long delta;
    const char *unit = units;
    int i;

    for (i = 0; i < ARRAY_SIZE(address_markers); i++)
        if (start == address_markers[i].start_address)
            printk("---[ %s ]---\n", address_markers[i].name);

    if (val == 0)
        return 0;

    printk("0x%016lx-0x%016lx   ", start, end);

    delta = (end - start) >> 10;
    while (!(delta & 1023) && units[1]) {
        delta >>= 10;
        unit++;
    }

    printk("%9lu%c %s", delta, *unit, pg_level[level].name);

    if (pg_level[level].bits)
        dump_prot(prot, pg_level[level].bits, pg_level[level].num);
    printk("\n");

    return 0;
}

static void walk_pte(pmd_t *pmdp, unsigned long start, unsigned long end)
{
    pte_t *ptep = pte_offset_phys(pmdp, start);
    unsigned long addr = start;

    do {
        print_pgtable(addr, addr + PAGE_SIZE, 4, pte_val(*ptep));
    } while (ptep++, addr += PAGE_SIZE, addr != end);
}

static void walk_pmd(pud_t *pudp, unsigned long start, unsigned long end)
{
    unsigned long next, addr = start;
    pmd_t *pmdp = pmd_offset_phys(pudp, start);
    pmd_t pmd;

    do {
        pmd = *pmdp;
        next = pmd_addr_end(addr, end);

        if (pmd_none(pmd) || pmd_sect(pmd)) {
            //printk("        PMD Entry(Empty): 0x%016lx-0x%016lx\n", addr, next);
            print_pgtable(addr, next, 3, pmd_val(pmd));
        } else {
            printk("        PMD Entry: 0x%016lx-0x%016lx\n", addr, next);
            walk_pte(pmdp, addr, next);
        }
    } while (pmdp++, addr = next, addr != end);
}

static void walk_pud(pgd_t *pgdp, unsigned long start, unsigned long end)
{
    unsigned long next, addr = start;
    pud_t *pudp = pud_offset_phys(pgdp, start);
    pud_t pud;

    do {
        pud = *pudp;
        next = pud_addr_end(start, end);

        if (pud_none(pud) || pud_sect(pud))
            print_pgtable(addr, next, 2, pud_val(pud));
        else {
            printk("    PUD Entry: 0x%016lx-0x%016lx\n", addr, next);
            walk_pmd(pudp, addr, next);
        }

    } while (pudp++, addr = next, addr != end);
}

void walk_pgd(pgd_t *pgd, unsigned long start, unsigned long size)
{
    unsigned long end = start + size;
    unsigned long next, addr = start;
    pgd_t *pgdp;
    pgd_t pgd_entry;

    pgdp = pgd_offset_raw(pgd, start);

    do {
        pgd_entry = *pgdp;
        next = pgd_addr_end(addr, end);

        if (pgd_none(pgd_entry))
            print_pgtable(addr, next, 1, pgd_val(pgd_entry));
        else {
            printk("PGD Entry: 0x%016lx-0x%016lx\n", addr, next);
            walk_pud(pgdp, addr, next);
        }
    } while (pgdp++, addr = next, addr != end);
}

static void pg_level_init(void)
{
    unsigned int i, j;

    for (i = 0; i < ARRAY_SIZE(pg_level); i++)
        if (pg_level[i].bits)
            for (j = 0; j < pg_level[i].num; j++)
                pg_level[i].mask |= pg_level[i].bits[j].mask;
}

static pte_t *walk_pgtable(unsigned long address)
{
    pgd_t *pgd = NULL;
    pud_t *pud;
    pte_t *pte;
    pmd_t *pmd;

    /* pgd */
    pgd = pgd_offset_raw((pgd_t *)idmap_pgd_dir, address);
    if (pgd == NULL || pgd_none(*pgd))
        return NULL;

    pud = pud_offset_phys(pgd, address);
    if (pud ==NULL || pud_none(*pud))
        return NULL;

    pmd = pmd_offset_phys(pud, address);
    if (pmd == NULL || pmd_none(*pmd))
        return NULL;
    else if ((pmd_val(*pmd) & PMD_TYPE_MASK) == PMD_TYPE_SECT) {
        return (pte_t *)pmd;
    }

    pte = pte_offset_phys(pmd, address);
    if ((pte == NULL) || pte_none(*pte))
        return NULL;

    return pte;
}

void test_walk_pgtable(void)
{
    pte_t pte, *ptep;
    pte_t pte_new;

    unsigned long addr = (unsigned long)readonly_page;

    ptep = walk_pgtable(addr);

    pte = *ptep;

    printk("Test Page:0x%lx, pte value: 0x%lx\n", addr, pte);

    pte_new = pte_mkyoung(pte);
    pte_new = pte_mkwrite(pte_new);

    set_pte(ptep, pte_new);

    /*dump addr's pte*/
    walk_pgd((pgd_t *)idmap_pgd_dir, addr, PAGE_SIZE);

    memset((void*)readonly_page, 0x55, 100);
    printk("write readonly page done\n");
}

void dump_pgtable(void)
{
    pg_level_init();
    walk_pgd((pgd_t *)idmap_pgd_dir, 0, TOTAL_MEMORY);
    walk_pgd((pgd_t *)idmap_pgd_dir, PBASE, DEVICE_SIZE);
}
