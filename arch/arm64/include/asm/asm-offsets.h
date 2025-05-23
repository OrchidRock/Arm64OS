#ifndef __ASM_OFFSETS_H__
#define __ASM_OFFSETS_H__

#define S_FRAME_SIZE 320 /* sizeof(struct pt_regs)	// */
#define S_X0 0 /* offsetof(struct pt_regs, regs[0])	// */
#define S_X1 8 /* offsetof(struct pt_regs, regs[1])	// */
#define S_X2 16 /* offsetof(struct pt_regs, regs[2])	// */
#define S_X3 24 /* offsetof(struct pt_regs, regs[3])	// */
#define S_X4 32 /* offsetof(struct pt_regs, regs[4])	// */
#define S_X5 40 /* offsetof(struct pt_regs, regs[5])	// */
#define S_X6 48 /* offsetof(struct pt_regs, regs[6])	// */
#define S_X7 56 /* offsetof(struct pt_regs, regs[7])	// */
#define S_X8 64 /* offsetof(struct pt_regs, regs[8])	// */
#define S_X10 80 /* offsetof(struct pt_regs, regs[10])	// */
#define S_X12 96 /* offsetof(struct pt_regs, regs[12])	// */
#define S_X14 112 /* offsetof(struct pt_regs, regs[14])	// */
#define S_X16 128 /* offsetof(struct pt_regs, regs[16])	// */
#define S_X18 144 /* offsetof(struct pt_regs, regs[18])	// */
#define S_X20 160 /* offsetof(struct pt_regs, regs[20])	// */
#define S_X22 176 /* offsetof(struct pt_regs, regs[22])	// */
#define S_X24 192 /* offsetof(struct pt_regs, regs[24])	// */
#define S_X26 208 /* offsetof(struct pt_regs, regs[26])	// */
#define S_X28 224 /* offsetof(struct pt_regs, regs[28])	// */
#define S_FP 232 /* offsetof(struct pt_regs, regs[29])	// */
#define S_LR 240 /* offsetof(struct pt_regs, regs[30])	// */
#define S_SP 248 /* offsetof(struct pt_regs, sp)	// */
#define S_PSTATE 264 /* offsetof(struct pt_regs, pstate)	// */
#define S_PC 256 /* offsetof(struct pt_regs, pc)	// */
#define S_ORIG_X0 272 /* offsetof(struct pt_regs, orig_x0)	// */
#define S_SYSCALLNO 280 /* offsetof(struct pt_regs, syscallno)	// */
#define S_ORIG_ADDR_LIMIT 288 /* offsetof(struct pt_regs, orig_addr_limit)	// */
#define S_STACKFRAME 304 /* offsetof(struct pt_regs, stackframe)	// */
#define THREAD_CPU_CONTEXT 16 /* offsetof(struct task_struct, cpu_context)	// */
#define TI_PREEMPT 148 /* offsetof(struct task_struct, preempt_count)	// */
#define NEED_RESCHED 144 /* offsetof(struct task_struct, need_resched)	// */

#endif
