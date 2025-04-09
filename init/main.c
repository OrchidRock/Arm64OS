#include "asm/pgtable_types.h"
#include "asm/sysregs.h"
#include "asm/mm.h"
#include "uart.h"
#include "irq.h"
#include "timer.h"
#include "asm/esr.h"
#include "mach/base.h"
#include "mach/timer.h"
#include "arm-gic.h"
#include "mmu.h"
#include "memory.h"
#include "printk.h"
#include "sched.h"
#include "usr/syscall.h"

extern void trigger_invalid_alignment(void);

extern char _text_boot_start[], _text_boot_end[];
extern char _text_start[], _text_end[];
extern char _rodata_start[], _rodata_end[];
extern char _data_start[], _data_end[];
extern char bss_begin[], bss_end[];

extern union task_union init_task_union;

static void print_mem(void)
{
    printk("Arm64OS image layout:\n");
    printk("  .text.boot: 0x%08lx - 0x%08lx (%6ld B)\n",
                    (unsigned long)_text_boot_start, (unsigned long)_text_boot_end,
                    (unsigned long)(_text_boot_end - _text_boot_start));
    printk("       .text: 0x%08lx - 0x%08lx (%6ld B)\n",
                    (unsigned long)_text_start, (unsigned long)_text_end,
                    (unsigned long)(_text_end - _text_start));
    printk("     .rodata: 0x%08lx - 0x%08lx (%6ld B)\n",
                    (unsigned long)_rodata_start, (unsigned long)_rodata_end,
                    (unsigned long)(_rodata_end - _rodata_start));
    printk("       .data: 0x%08lx - 0x%08lx (%6ld B)\n",
                    (unsigned long)_data_start, (unsigned long)_data_end,
                    (unsigned long)(_data_end - _data_start));
    printk("        .bss: 0x%08lx - 0x%08lx (%6ld B)\n",
                    (unsigned long)bss_begin, (unsigned long)bss_end,
                    (unsigned long)(bss_end - bss_begin));
}

static int test_access_map_address(void)
{
	unsigned long address = TOTAL_MEMORY - 4096;
	*(unsigned long *)address = 0x55;
	printk("%s access 0x%x done\n", __func__, address);
	return 0;
}

static int test_access_unmap_address(void)
{
	unsigned long address = TOTAL_MEMORY + 4096;
	*(unsigned long *)address = 0x55;
	printk("%s access 0x%x done\n", __func__, address);
	return 0;
}

extern unsigned long mmu_at_test(unsigned long addr);

static unsigned long parse_par_el1(unsigned long val)
{
	int ret = val & 1;
	printk("Address Translation instruction %s\n", (ret == 0) ? "successfully" : "failed");
	if (ret)
		return -1;

	unsigned long pa = (val & PAGE_MASK) & 0xffffffffff;
	printk("par_el1:0x%lx, PA: 0x%lx\n", val, pa);

	printk("  NS=%u\n", (val >> 9) & 1);
	printk("  SH=%u\n", (val >> 7) & 3);
	printk("  ATTR=0x%x\n", val >> 56);

	return pa;
}

static void test_mmu(void)
{
  	unsigned long par_el1;
	unsigned long pa;

  	unsigned long addr = TOTAL_MEMORY - 4096;
	par_el1 = mmu_at_test(addr);
	pa = parse_par_el1(par_el1);
	printk("test AT instruction %s\n", (addr == pa) ? "done" : "failed");

 	addr = TOTAL_MEMORY + 4096;
	par_el1 = mmu_at_test(addr);
	pa = parse_par_el1(par_el1);
	printk("test AT instruction %s\n", (addr == pa) ? "done" : "failed");

	test_access_map_address();
	test_access_unmap_address();
}

register unsigned long current_stack_pointer asm ("sp");

int kernel_thread1(void *arg)
{
	while (1) {
		delay(80000);
		printk("%s: %s\n", __func__, "12345");
	}
    return 0;
}

int kernel_thread2(void *arg)
{
	while (1) {
		delay(50000);
		printk("%s: %s\n", __func__, "abcde");
	}
    return 0;
}

int run_user_thread(void* arg)
{
    unsigned long child_stack;
    //printk("%s: running at EL%d\n", __func__, read_sysreg(CurrentEL) >> 2);
    int my_local_var = 0;
    child_stack = my_malloc();
    while (1) {
       delay(50000);
       my_local_var += 1;
       //printk("%s: runing at userspace\n", __func__);
    }
    return 0;
}

int user_thread(void *arg)
{
    printk("%s: running at EL%d\n", __func__, read_sysreg(CurrentEL) >> 2);
    if (move_to_user_space((unsigned long)&run_user_thread))
        printk("error move_to_user_space\n");

    return 0;
}

void start_kernel(void)
{
    uart_init();
    uart_send_string("Hello World from Arm64OS!\r\n");

    init_printk_done();
    printk("printk init done.\n");

    printk("Running on EL:");
	int el = read_sysreg(CurrentEL) >> 2;
	printk("%d\n", el);

    arch_irq_init();

    mem_init((unsigned long)bss_end, TOTAL_MEMORY);
	sched_init();
    print_mem();

    //trigger_invalid_alignment();

	printk("init(0) thread's task_struct address: 0x%lx\n", &init_task_union.task);
	printk("the SP of 0 thread: 0x%lx\n", current_stack_pointer);

    paging_init();
    printk("Page Table Init Done.\n");

#ifdef DEBUG_DUMP_PGTABLE
    dump_pgtable();
    test_walk_pgtable();
#endif

#ifdef INIT_CACHE_INFO
    init_cache_info();
#endif

    //test_mmu();

    timer_init();
    //system_timer_init();
    raw_local_irq_enable();

	int pid;

	pid = kernel_thread(kernel_thread1, 0, 0);
	if (pid < 0)
		printk("create thread fail\n");

	pid = kernel_thread(kernel_thread2, 0, 0);
	if (pid < 0)
		printk("create thread fail\n");

    /* Create User thread. */
//    pid = kernel_thread(user_thread, 0, 0);
//    if (pid < 0)
//        printk("create thread fail\n");

    char buffer[64] = {0};
    while(1) {
        uart_send_string("> ");
        uart_recv_string(buffer, sizeof(buffer));
        uart_send_string(buffer);
        uart_send_string("\r\n");
    }
}

