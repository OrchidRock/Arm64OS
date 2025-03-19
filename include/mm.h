//
// Created by mimose on 25-3-19.
//

#ifndef MM_H
#define MM_H


#define PAGE_SHIFT 12
#define TABLE_SHIFT 9
#define SECTION_SHIFT (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SECTION_SHIFT)

#define LOW_MEMORY (2 * SECTION_SIZE)    // 4M

#ifndef __ASSEMBLER__
void memzero(unsigned long src, unsigned long len);
#endif

#endif //MM_H
