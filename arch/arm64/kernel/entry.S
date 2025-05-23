#include "asm/asm-offsets.h"
#include "asm/mm.h"
#include "asm/esr.h"

#define THREAD_SIZE_MASK (~())
#define BAD_SYNC    0
#define BAD_IRQ     1
#define BAD_FIQ     2
#define BAD_ERROR   3

/* macro define */

.macro	disable_irq
	msr	daifset, #2
.endm

.macro inv_entry el, reason
    mov x0, sp
    mov x1, #\reason
    mrs x2, esr_el1
    b bad_mode
.endm

.macro kernel_ventry label
    .align 7
    b \label
.endm

#if 1
.macro kernel_context_save el
    sub sp, sp, #S_FRAME_SIZE
    // save general register: x0 - x29
    stp x0, x1, [sp, #16 *0]
    stp x2, x3, [sp, #16 *1]
    stp x4, x5, [sp, #16 *2]
    stp x6, x7, [sp, #16 *3]
    stp x8, x9, [sp, #16 *4]
    stp x10, x11, [sp, #16 *5]
    stp x12, x13, [sp, #16 *6]
    stp x14, x15, [sp, #16 *7]
    stp x16, x17, [sp, #16 *8]
    stp x18, x19, [sp, #16 *9]
    stp x20, x21, [sp, #16 *10]
    stp x22, x23, [sp, #16 *11]
    stp x24, x25, [sp, #16 *12]
    stp x26, x27, [sp, #16 *13]
    stp x28, x29, [sp, #16 *14]

.if \el == 0
	mrs x21, sp_el0
.else
	/* x21: get stack top */
	add     x21, sp, #S_FRAME_SIZE
.endif

    // get ELR reg
    mrs     x22, elr_el1

    // get spsr reg
    mrs     x23, spsr_el1

    // save lr and sp
    stp     lr, x21, [sp, #S_LR]

    // save pc and pstate
    stp     x22, x23, [sp, #S_PC]
.endm

.macro kernel_context_res el
    //load ELR, SPSR
    ldp     x21, x22, [sp, #S_PC]
.if \el == 0
	ldr     x23, [sp, #S_SP]
	msr     sp_el0, x23
.endif

    msr     elr_el1, x21
    msr     spsr_el1, x22
    ldp     x0, x1, [sp, #16 * 0]
    ldp     x2, x3, [sp, #16 * 1]
    ldp     x4, x5, [sp, #16 * 2]
    ldp     x6, x7, [sp, #16 * 3]
    ldp     x8, x9, [sp, #16 * 4]
    ldp     x10, x11, [sp, #16 * 5]
    ldp     x12, x13, [sp, #16 * 6]
    ldp     x14, x15, [sp, #16 * 7]
    ldp     x16, x17, [sp, #16 * 8]
    ldp     x18, x19, [sp, #16 * 9]
    ldp     x20, x21, [sp, #16 * 10]
    ldp     x22, x23, [sp, #16 * 11]
    ldp     x24, x25, [sp, #16 * 12]
    ldp     x26, x27, [sp, #16 * 13]
    ldp     x28, x29, [sp, #16 * 14]

    ldr     lr, [sp, #S_LR]
    add     sp, sp, #S_FRAME_SIZE

    eret
.endm
#endif

#if 0
kernel_context_save:
    sub sp, sp, #S_FRAME_SIZE
    // save general register: x0 - x29
    stp x0, x1, [sp, #16 *0]
    stp x2, x3, [sp, #16 *1]
    stp x4, x5, [sp, #16 *2]
    stp x6, x7, [sp, #16 *3]
    stp x8, x9, [sp, #16 *4]
    stp x10, x11, [sp, #16 *5]
    stp x12, x13, [sp, #16 *6]
    stp x14, x15, [sp, #16 *7]
    stp x16, x17, [sp, #16 *8]
    stp x18, x19, [sp, #16 *9]
    stp x20, x21, [sp, #16 *10]
    stp x22, x23, [sp, #16 *11]
    stp x24, x25, [sp, #16 *12]
    stp x26, x27, [sp, #16 *13]
    stp x28, x29, [sp, #16 *14]

    // get stack top
    add     x21, sp, #S_FRAME_SIZE

    // get ELR reg
    mrs     x22, elr_el1

    // get spsr reg
    mrs     x23, spsr_el1

    // save lr and sp
    stp     lr, x21, [sp, #S_LR]

    // save pc and pstate
    stp     x22, x23, [sp, #S_PC]
    ret

kernel_context_res:
    //load ELR, SPSR
    ldp     x21, x22, [sp, #S_PC]
    cbnz    x0, RES_EL1
    ldr     x23, [sp, #S_SP]
    msr     sp_el0, x23

RES_EL1:
    msr     elr_el1, x21
    msr     spsr_el1, x22
    ldp     x0, x1, [sp, #16 * 0]
    ldp     x2, x3, [sp, #16 * 1]
    ldp     x4, x5, [sp, #16 * 2]
    ldp     x6, x7, [sp, #16 * 3]
    ldp     x8, x9, [sp, #16 * 4]
    ldp     x10, x11, [sp, #16 * 5]
    ldp     x12, x13, [sp, #16 * 6]
    ldp     x14, x15, [sp, #16 * 7]
    ldp     x16, x17, [sp, #16 * 8]
    ldp     x18, x19, [sp, #16 * 9]
    ldp     x20, x21, [sp, #16 * 10]
    ldp     x22, x23, [sp, #16 * 11]
    ldp     x24, x25, [sp, #16 * 12]
    ldp     x26, x27, [sp, #16 * 13]
    ldp     x28, x29, [sp, #16 * 14]

    ldr     lr, [sp, #S_LR]
    add     sp, sp, #S_FRAME_SIZE

    ldr x30, [sp], 8
    eret
    ret

#endif

/* Vector Table (2048 bytes) */
.align 11
.global vectors
vectors:
    /* Current EL with SP0 */
    kernel_ventry el1_sync_invalid
    kernel_ventry el1_irq_invalid
    kernel_ventry el1_fiq_invalid
    kernel_ventry el1_error_invalid


    /* Current EL with SPx */
    kernel_ventry el1_sync_invalid
    kernel_ventry el1_irq
    kernel_ventry el1_fiq_invalid
    kernel_ventry el1_error_invalid

    /* Lower EL using AArch64 */
    kernel_ventry el0_sync
    kernel_ventry el0_irq
    kernel_ventry el0_fiq_invalid
    kernel_ventry el0_error_invalid


    /* Lower EL using AArch32 */
    kernel_ventry el0_sync_invalid
    kernel_ventry el0_irq_invalid
    kernel_ventry el0_fiq_invalid
    kernel_ventry el0_error_invalid

el1_sync_invalid:
    //inv_entry 1, BAD_SYNC
    kernel_context_save 1
    mov x0, sp
    mov x1, 0
    mrs x2, esr_el1
    bl bad_mode
    kernel_context_res 1

el1_irq_invalid:
    inv_entry 1, BAD_IRQ

el1_fiq_invalid:
    inv_entry 1, BAD_FIQ

el1_error_invalid:
    inv_entry 1, BAD_ERROR

el0_sync_invalid:
	inv_entry 0, BAD_SYNC

el0_irq_invalid:
    inv_entry 0, BAD_IRQ

el0_fiq_invalid:
    inv_entry 0, BAD_FIQ

el0_error_invalid:
    inv_entry 0, BAD_ERROR

tsk     .req    x28             // current thread_info


.align 3
el0_sync:
    kernel_context_save 0
   	mrs  x25, esr_el1
   	lsr x24, x25, #ESR_ELx_EC_SHIFT
   	cmp x24, #ESR_ELx_EC_SVC64
   	b.eq    el0_svc

el0_svc:
	mov x0, sp
	bl el0_svc_handler
	b ret_to_user

el0_irq:
    kernel_context_save 0
    bl irq_handler
    b ret_to_user

.align 2
el1_irq:
    kernel_context_save 1
    bl irq_handler

    mov tsk, sp
    mov x27, #THREAD_SIZE
    sub x27, x27, #1
    mvn x27, x27
    and tsk, tsk, x27

    ldr  w24, [tsk, #TI_PREEMPT]
    cbnz w24, 1f
    ldr  w0, [tsk, #NEED_RESCHED]
    cbz w0, 1f
    bl el1_preempt
1:
    kernel_context_res 1

el1_preempt:
	mov     x24, lr
	bl preempt_schedule_irq
	ret     x24


#if 0
string_test:
    .string "t"

.global trigger_invalid_alignment
trigger_invalid_alignment:
    ldr x0, =0x80002
    ldr x1, [x0]
    ret
#endif

.align 2
.global ret_to_user
ret_to_user:
    disable_irq
	//inv_entry 0, BAD_ERROR
    mov tsk, sp
    mov x27, #THREAD_SIZE
    sub x27, x27, #1
    mvn x27, x27
    and tsk, tsk, x27
    ldr  w24, [tsk, #NEED_RESCHED]
    cbnz w24, need_sched

no_work_pending:
	kernel_context_res 0

need_sched:
	bl schedule
    b no_work_pending


.align 2
.global ret_from_fork
ret_from_fork:
    bl schedule_tail
    cbz x19, 1f
    mov x0, x20
    blr x19
1:
	b ret_to_user

.align
.global cpu_switch_to
cpu_switch_to:
    add     x8, x0, #THREAD_CPU_CONTEXT
    mov     x9, sp
    stp     x19, x20, [x8], #16
    stp     x21, x22, [x8], #16
    stp     x23, x24, [x8], #16
    stp     x25, x26, [x8], #16
    stp     x27, x28, [x8], #16
    stp     x29, x9, [x8], #16
    str     lr, [x8]

    add     x8, x1, #THREAD_CPU_CONTEXT
    ldp     x19, x20, [x8], #16
    ldp     x21, x22, [x8], #16
    ldp     x23, x24, [x8], #16
    ldp     x25, x26, [x8], #16
    ldp     x27, x28, [x8], #16
    ldp     x29, x9, [x8], #16
    ldr     lr, [x8]
    mov     sp, x9
    ret

