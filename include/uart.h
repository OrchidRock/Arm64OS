#ifndef UART_H
#define UART_H

void uart_init(void);
char uart_recv(void);
void uart_send(char c);
void uart_send_string(const char* str);

#endif //UART_H
