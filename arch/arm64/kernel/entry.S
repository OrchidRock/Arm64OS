
#include "asm/asm-offsets.h"

#define BAD_SYNC    0
#define BAD_IRQ     1
#define BAD_FIQ     2
#define BAD_ERROR   3

/* macro define */

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

#if 0
.macro kernel_context_save
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
.endm

.macro kernel_context_res
    //load ELR, SPSR
    ldp     x21, x22, [sp, #S_PC]
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
    kernel_ventry el0_sync_invalid
    kernel_ventry el0_irq_invalid
    kernel_ventry el0_fiq_invalid
    kernel_ventry el0_error_invalid


    /* Lower EL using AArch32 */
    kernel_ventry el0_sync_invalid
    kernel_ventry el0_irq_invalid
    kernel_ventry el0_fiq_invalid
    kernel_ventry el0_error_invalid

el1_sync_invalid:
    //inv_entry 1, BAD_SYNC
    bl kernel_context_save
    mov x0, sp
    mov x1, 0
    mrs x2, esr_el1
    bl bad_mode
    bl kernel_context_res

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

el1_irq:
    str x30, [sp, -8]!
    bl kernel_context_save
    bl irq_handler
    bl kernel_context_res

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
	inv_entry 0, BAD_ERROR

.align 2
.global ret_from_fork
ret_from_fork:
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

