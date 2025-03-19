
#ifndef PL_UART_H
#define PL_UART_H

#include "base.h"

#define UART_BASE       (PBASE+0x00201000)

#define UART_DATA_REG   (UART_BASE)
#define UART_FR_REG     (UART_BASE+0x18)
#define UART_IBRD_REG   (UART_BASE+0x24)
#define UART_FBRD_REG   (UART_BASE+0x28)
#define UART_LCRH_REG   (UART_BASE+0x2C)
#define UART_CR_REG     (UART_BASE+0x30)
#define UART_IMSC_REG   (UART_BASE+0x38)

#define UART_FR_RXFE    (1 << 4)
#define UART_FR_TXFF     (1 << 5)

#endif //PL_UART_H
