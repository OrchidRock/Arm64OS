#include "asm/mm.h"
#include "asm/sysregs.h"

.align 3
.globl sp_addr
sp_addr:
	.quad init_task_union + THREAD_START_SP

.section ".text.boot"
.global _start
_start:
        mrs x0, mpidr_el1
        and x0, x0, #0xFF
        cbz x0, core0_proc
        b hang_proc


core0_proc:
	mrs x5, CurrentEL
        mrs x1, sctlr_el1
	cmp x5, #CurrentEL_EL3
	b.eq el3_entry
	b el2_entry	

el3_entry:
	eret

el2_entry:
	# set the execution state for EL1 is AArch64
	ldr x0, =HCR_HOST_NVHE_FLAGS
	msr hcr_el2, x0

	# disable MMU
	ldr x0, =SCTLR_VALUE_MMU_DISABLED
	msr sctlr_el1, x0	

	# set spsr_el2 to return EL1
	ldr x0, =SPSR_EL1
	msr spsr_el2, x0

	adr x0, el1_entry
	msr elr_el2, x0

	eret

el1_entry:
	mrs x5, CurrentEL

    /* set vbar reg */
    ldr x5, =vectors
    msr vbar_el1, x5

    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero

    ldr x8, =sp_addr
    ldr x9, [x8]
    mov sp, x9
    bl start_kernel
    b hang_proc

hang_proc:
        b hang_proc
