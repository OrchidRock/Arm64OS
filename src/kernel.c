
#include "uart.h"

extern void trigger_invalid_alignment(void);

#define read_sysreg(reg) ({ \
        unsigned long __val; \
        asm volatile("mrs %0," #reg \
                      : "=r"(__val)); \
        __val;\ 
})

static const char * const bad_mode_handler[] = {
    "Sync Abort",
    "IRQ",
    "FIQ",
    "SError"
};

void bad_mode(struct pt_regs *regs, int reason, unsigned int esr)
{
    printk("Bad mode for %s handler detected, FAR: 0x%x esr: 0x%x\n",
                    bad_mode_handler[reason], read_sysreg(far_el1), esr);
}

extern char _text_boot_start[], _text_boot_end[];
extern char _text_start[], _text_end[];
extern char _rodata_start[], _rodata_end[];
extern char _data_start[], _data_end[];
extern char bss_begin[], bss_end[];

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

void start_kernel(void)
{
    uart_init();
    uart_send_string("Hello World from Arm64OS!\r\n");

    init_printk_done();
    printk("printk init done.\n");
   
    print_mem(); 

    trigger_invalid_alignment();

    char buffer[64] = {0};
    while(1) {
        uart_send_string("> ");
        uart_recv_string(buffer, sizeof(buffer));
        uart_send_string(buffer);
        uart_send_string("\r\n");
    }
}

