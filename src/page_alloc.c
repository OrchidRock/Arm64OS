#include "mm.h"

#define NR_PAGES (TOTAL_MEMORY / PAGE_SIZE)

static unsigned char mem_map[NR_PAGES] = {0,};
static unsigned long phy_start_address;

unsigned long get_free_page(void)
{
    for (int i = 0; i < NR_PAGES; i++) {
        if (mem_map[i] == 0) {
            mem_map[i] = 1;
            return LOW_MEMORY + i * PAGE_SIZE;
        }
    }
    return 0;
}

void free_page(unsigned long p)
{
    mem_map[(p - LOW_MEMORY)/PAGE_SIZE] = 0;
}
