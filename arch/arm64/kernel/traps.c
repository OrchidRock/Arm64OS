#include "printk.h"
#include "esr.h"
#include "asm/system.h"
#include "asm/sysregs.h"

static const char * const bad_mode_handler[] = {
    "Sync Abort",
    "IRQ",
    "FIQ",
    "SError"
};

static const char *ESR_CLASS_STR[] = {
    [0 ... ESR_ELx_EC_MAX]		= "UNRECOGNIZED EC",
    [ESR_ELx_EC_UNKNOWN]		= "Unknown/Uncategorized",
    [ESR_ELx_EC_WFx]		= "WFI/WFE",
    [ESR_ELx_EC_CP15_32]		= "CP15 MCR/MRC",
    [ESR_ELx_EC_CP15_64]		= "CP15 MCRR/MRRC",
    [ESR_ELx_EC_CP14_MR]		= "CP14 MCR/MRC",
    [ESR_ELx_EC_CP14_LS]		= "CP14 LDC/STC",
    [ESR_ELx_EC_FP_ASIMD]		= "ASIMD",
    [ESR_ELx_EC_CP10_ID]		= "CP10 MRC/VMRS",
    [ESR_ELx_EC_CP14_64]		= "CP14 MCRR/MRRC",
    [ESR_ELx_EC_ILL]		= "PSTATE.IL",
    [ESR_ELx_EC_SVC32]		= "SVC (AArch32)",
    [ESR_ELx_EC_HVC32]		= "HVC (AArch32)",
    [ESR_ELx_EC_SMC32]		= "SMC (AArch32)",
    [ESR_ELx_EC_SVC64]		= "SVC (AArch64)",
    [ESR_ELx_EC_HVC64]		= "HVC (AArch64)",
    [ESR_ELx_EC_SMC64]		= "SMC (AArch64)",
    [ESR_ELx_EC_SYS64]		= "MSR/MRS (AArch64)",
    [ESR_ELx_EC_IMP_DEF]		= "EL3 IMP DEF",
    [ESR_ELx_EC_IABT_LOW]		= "IABT (lower EL)",
    [ESR_ELx_EC_IABT_CUR]		= "IABT (current EL)",
    [ESR_ELx_EC_PC_ALIGN]		= "PC Alignment",
    [ESR_ELx_EC_DABT_LOW]		= "DABT (lower EL)",
    [ESR_ELx_EC_DABT_CUR]		= "DABT (current EL)",
    [ESR_ELx_EC_SP_ALIGN]		= "SP Alignment",
    [ESR_ELx_EC_FP_EXC32]		= "FP (AArch32)",
    [ESR_ELx_EC_FP_EXC64]		= "FP (AArch64)",
    [ESR_ELx_EC_SERROR]		= "SError",
    [ESR_ELx_EC_BREAKPT_LOW]	= "Breakpoint (lower EL)",
    [ESR_ELx_EC_BREAKPT_CUR]	= "Breakpoint (current EL)",
    [ESR_ELx_EC_SOFTSTP_LOW]	= "Software Step (lower EL)",
    [ESR_ELx_EC_SOFTSTP_CUR]	= "Software Step (current EL)",
    [ESR_ELx_EC_WATCHPT_LOW]	= "Watchpoint (lower EL)",
    [ESR_ELx_EC_WATCHPT_CUR]	= "Watchpoint (current EL)",
    [ESR_ELx_EC_BKPT32]		= "BKPT (AArch32)",
    [ESR_ELx_EC_VECTOR32]		= "Vector catch (AArch32)",
    [ESR_ELx_EC_BRK64]		= "BRK (AArch64)",
};

static const char *esr_get_class_string(unsigned int esr)
{
    return ESR_CLASS_STR[esr >> ESR_ELx_EC_SHIFT];
}

static const char *DATA_FAULT_CODE[] = {
    [0] = "Address size fault, level0",
    [1] = "Address size fault, level1",
    [2] = "Address size fault, level2",
    [3] = "Address size fault, level3",
    [4] = "Translation fault, level0",
    [5] = "Translation fault, level1",
    [6] = "Translation fault, level2",
    [7] = "Translation fault, level3",
    [9] = "Access flag fault, level1",
    [10] = "Access flag fault, level2",
    [11] = "Access flag fault, level3",
    [13] = "Permission fault, level1",
    [14] = "Permission fault, level2",
    [15] = "Permission fault, level3",
    [0x21] = "Alignment fault",
    [0x35] = "Unsupported Exclusive or Atomic access",
};

static const char *esr_get_dfsc_string(unsigned int esr)
{
    return DATA_FAULT_CODE[esr & 0x3f];
}



void parse_esr(unsigned int esr)
{
    unsigned int ec = ESR_ELx_EC(esr);

    printk("ESR info:\n");
    printk("  ESR = 0x%08x\n", esr);
    printk("  Exception class = %s, IL = %u bits\n",
         esr_get_class_string(esr),
         (esr & ESR_ELx_IL) ? 32 : 16);


    if (ec == ESR_ELx_EC_DABT_LOW || ec == ESR_ELx_EC_DABT_CUR) {
        printk("  Data abort:\n");
        if ((esr & ESR_ELx_ISV)) {
            printk("  Access size = %u byte(s)\n",
             1U << ((esr & ESR_ELx_SAS) >> ESR_ELx_SAS_SHIFT));
            printk("  SSE = %lu, SRT = %lu\n",
             (esr & ESR_ELx_SSE) >> ESR_ELx_SSE_SHIFT,
             (esr & ESR_ELx_SRT_MASK) >> ESR_ELx_SRT_SHIFT);
            printk("  SF = %lu, AR = %lu\n",
             (esr & ESR_ELx_SF) >> ESR_ELx_SF_SHIFT,
             (esr & ESR_ELx_AR) >> ESR_ELx_AR_SHIFT);
        }

        printk("  SET = %lu, FnV = %lu\n",
            (esr >> ESR_ELx_SET_SHIFT) & 3,
            (esr >> ESR_ELx_FnV_SHIFT) & 1);
        printk("  EA = %lu, S1PTW = %lu\n",
            (esr >> ESR_ELx_EA_SHIFT) & 1,
            (esr >> ESR_ELx_S1PTW_SHIFT) & 1);
        printk("  CM = %lu, WnR = %lu\n",
         (esr & ESR_ELx_CM) >> ESR_ELx_CM_SHIFT,
         (esr & ESR_ELx_WNR) >> ESR_ELx_WNR_SHIFT);
        printk("  DFSC = %s\n", esr_get_dfsc_string(esr));
    }
}

void panic(void)
{
    printk("Kernel panic\n");
    while (1)
        ;
}

void bad_mode(struct pt_regs *regs, int reason, unsigned int esr)
{
    printk("Bad mode for %s handler detected, FAR: 0x%x esr: 0x%x - %s\n",
                    bad_mode_handler[reason], read_sysreg(far_el1),
                    esr, esr_get_class_string(esr));
    parse_esr(esr);

    panic();
}
