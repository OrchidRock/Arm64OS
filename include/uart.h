#ifndef UART_H
#define UART_H

void uart_init(void);
char uart_recv(void);
void uart_send(char c);
void uart_send_string(const char* str);
int uart_recv_string(char* buf, int len);

#endif //UART_H
