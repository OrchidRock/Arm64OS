//
// Created by mimose on 25-3-19.
//

#ifndef GPIO_H
#define GPIO_H

#include "asm/base.h"

#define GPFSEL1      (PBASE + 0x00200004U)
#define GPFSEL0      (PBASE + 0x0020001CU)
#define GPCLR0       (PBASE + 0x00200028U)
#define GPPUD        (PBASE + 0x00200094U)
#define GPPUDCLK0    (PBASE + 0x00200098U)

#endif //GPIO_H
