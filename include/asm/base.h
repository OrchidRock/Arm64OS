#ifndef BASE_H
#define BASE_H


#ifndef RPI5_BCM2712
/* Main peripherals on Low Peripheral mode
 * - ARM and GPU can access
 * see <BCM2711 ARM Peripherals> 1.2.4
 */
#define PBASE (0xFE000000U)

/*
 * ARM LOCAL register on Low Peripheral mode
 * - Only ARM can access
 * see <BCM2711 ARM Peripherals> 6.5.2
 */
#define ARM_LOCAL_BASE 0xFF800000

#else
#define PBASE 0x107C000000UL
#endif

#endif //BASE_H
