//
// Created by mimose on 25-3-23.
//

#ifndef PAGE_H
#define PAGE_H

#define PAGE_SHIFT     12
#define TABLE_SHIFT    9
#define SECTION_SHIFT  (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE     (1 << PAGE_SHIFT)
#define PAGE_MASK     (~(PAGE_SIZE-1))
#define SECTION_SIZE  (1 << SECTION_SHIFT)

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)         (((addr)+PAGE_SIZE-1)&PAGE_MASK)
#define PAGE_ALIGN_UP(addr)      PAGE_ALIGN(addr)
#define PAGE_ALIGN_DOWN(addr)    (addr & PAGE_MASK)

#endif //PAGE_H
