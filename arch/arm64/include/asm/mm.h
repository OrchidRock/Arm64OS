#ifndef ARCH_ASM_MM_H
#define ARCH_ASM_MM_H

#include "page.h"
#include "std_types.h"

#define LOW_MEMORY    (2 * SECTION_SIZE)    // 4M
#define TOTAL_MEMORY  (512 * 0x100000)

#ifndef __ASSEMBLER__
void memzero(unsigned long src, unsigned long len);
#endif

#define CONFIG_ARM64_VA_BITS 48
#define VA_BITS	 (CONFIG_ARM64_VA_BITS)

/*
 * Memory attiture available.
 */
#define MT_DEVICE_nGnRnE	0
#define MT_DEVICE_nGnRE		1
#define MT_DEVICE_GRE		2
#define MT_NORMAL_NC		3
#define MT_NORMAL		    4
#define MT_NORMAL_WT		5

#define MAIR(attr, mt)	    ((attr) << ((mt) * 8))


#define THREAD_SIZE  (1 * PAGE_SIZE)
#define THREAD_START_SP  (THREAD_SIZE - 16)

#endif //MM_H
