#include "asm/pl_uart.h"
#include "asm/gpio.h"
#include "io.h"

//void delay(unsigned long loops);
static inline void delay(unsigned long loops)
{
    while (loops--)
        ;
}

void uart_init(void)
{
#ifndef RPI5_BCM2712
    unsigned int selector;

    /* clean gpio4 */
    selector = readl(GPFSEL1);
    selector &= ~(7<<12);
    /* set alt0 for gpio14 */
    selector |= 4<<12;
    /* clean gpio15 */
    selector &= ~(7<<15);
    /* set alt0 for gpio15 */
    selector |= 4<<15;
    writel(selector, GPFSEL1);

    writel(0, GPPUD);
    delay(150);
    writel((1<<14) | (1<<15), GPPUDCLK0);
    delay(150);
    writel(0, GPPUDCLK0);
#endif

    /* disable UART until configuration is done */
    writel(0, UART_CR_REG);

    writel(0x7FF, UART_ICR_REG);

    /*
     * baud divisor = UARTCLK / (16 * baud_rate)
    = 48 * 10^6 / (16 * 115200) = 26.0416666667
    integer part = 26
    fractional part = (int) ((0.0416666667 * 64) + 0.5) = 3
    generated baud rate divisor = 26 + (3 / 64) = 26.046875
    generated baud rate = (48 * 10^6) / (16 * 26.046875) = 115177
    error = |(115177 - 115200) / 115200 * 100| = 0.02%
    */

    /* baud rate divisor, integer part */
    writel(26, UART_IBRD_REG);
    /* baud rate divisor, fractional part */
    writel(3, UART_FBRD_REG);

    /* enable FIFOs and 8 bits frames */
    writel((1<<4) | (3<<5), UART_LCRH_REG);

    /* mask interupts */
    writel(0, UART_IMSC_REG);
    /* enable UART, receive and transmit */
    writel(1 | (1<<8) | (1<<9), UART_CR_REG);
}

char uart_recv(void)
{
    while (readl(UART_FR_REG) & UART_FR_RXFE)
        ;
    return (readl(UART_DATA_REG) & 0xFF);
}

void uart_send(char c)
{
    while (readl(UART_FR_REG) & UART_FR_TXFF)
        ;
    writel(c, UART_DATA_REG);
}

void uart_send_string(const char* str)
{
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        uart_send((char)str[i]);
    }
}

int uart_recv_string(char* buf, int len)
{
    int index = 0;
    char c;
    while (index < len) {
        c = uart_recv();
        buf[index++] = c;
        if (c == '\r' || c == '\n') {
            break;
        }
    }
    if (index == len) {
        index -= 1;
    }
    buf[index] = '\0';
    return index;
}

void putchar(char c)
{
    if (c == '\n')
        uart_send('\r');
    uart_send(c);    
}
