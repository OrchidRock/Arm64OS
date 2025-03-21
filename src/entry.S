
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
    kernel_ventry el1_irq_invalid
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
    inv_entry 1, BAD_SYNC

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


string_test:
    .string "t"

.global trigger_invalid_alignment
trigger_invalid_alignment:
    ldr x0, =0x80002
    ldr x1, [x0]
    ret

