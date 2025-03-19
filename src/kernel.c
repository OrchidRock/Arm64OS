
#include "uart.h"

void start_kernel(void)
{
    uart_init();
    uart_send_string("Hello World from Arm64OS!\r\n");

    char buffer[64] = {0};
    while(1) {
        uart_send_string("> ");
        uart_recv_string(buffer, sizeof(buffer));
        uart_send_string(buffer);
        uart_send_string("\r\n");
    }
}

