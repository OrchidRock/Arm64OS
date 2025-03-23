#include "asm/pgtable_types.h"
#include "asm/mm.h"
#include "uart.h"
#include "irq.h"
#include "timer.h"
#include "esr.h"
#include "mach/base.h"
#include "arm-gic.h"
#include "mmu.h"
#include "memory.h"
#include "printk.h"
#include "sched.h"

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

static void test_mmu(void)
{
	test_access_map_address();
	test_access_unmap_address();
}

register unsigned long current_stack_pointer asm ("sp");

void kernel_thread(void)
{
	while (1) {
		delay(100000000);
		printk("%s: %s\n", __func__, "12345");
	}
}

void start_kernel(void)
{
    uart_init();
    uart_send_string("Hello World from Arm64OS!\r\n");

    init_printk_done();
    printk("printk init done.\n");

    mem_init((unsigned long)bss_end, TOTAL_MEMORY);
    print_mem(); 

    //trigger_invalid_alignment();

	printk("init(0) thread's task_struct address: 0x%lx\n", &init_task_union.task);
	printk("the SP of 0 thread: 0x%lx\n", current_stack_pointer);

    printk("done\n");

    paging_init();

    dump_pgtable();
    test_walk_pgtable();
	//test_mmu();

	gic_init(0, GIC_V2_DISTRIBUTOR_BASE, GIC_V2_CPU_INTERFACE_BASE);
    timer_init();
    //system_timer_init();
    raw_local_irq_enable();

    int pid = do_fork(PF_KTHREAD, (unsigned long)&kernel_thread, 0);
	if (pid < 0)
		printk("create thread fail\n");
    else {
    	printk("create kernel thread: %d\n", pid);
    }

	struct task_struct *next = g_task[pid];
	switch_to(next);

    char buffer[64] = {0};
    while(1) {
        uart_send_string("> ");
        uart_recv_string(buffer, sizeof(buffer));
        uart_send_string(buffer);
        uart_send_string("\r\n");
    }
}

