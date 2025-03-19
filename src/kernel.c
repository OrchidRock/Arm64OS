
#include "uart.h"

void start_kernel(void)
{
    uart_init();
    uart_send_string("Hello World from Arm64OS!\r\n");

    while(1) {
        uart_send(uart_recv());
    }
}

