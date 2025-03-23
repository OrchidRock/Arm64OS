#ifndef MEMORY_H
#define MEMORY_H

unsigned long get_free_page(void);
void free_page(unsigned long p);
void mem_init(unsigned long start_mem, unsigned long end_mem);

#endif //MEMORY_H
