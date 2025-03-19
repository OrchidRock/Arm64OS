
void uart_init(void)
{

}

char uart_recv(void)
{
    return 0;
}

void uart_send(char c)
{

}

void uart_send_string(const char* str)
{
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        uart_send((char)str[i]);
    }
}