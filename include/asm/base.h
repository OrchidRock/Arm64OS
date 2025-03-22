#ifndef BASE_H
#define BASE_H


#ifndef RPI5_BCM2712
/* Main peripherals on Low Peripheral mode
 * - ARM and GPU can access
 * see <BCM2711 ARM Peripherals> 1.2.4
 */
#define PBASE          (0xFE000000U)
#define PBASE_END      (0x100000000U)

/*
 * ARM LOCAL register on Low Peripheral mode
 * - Only ARM can access
 * see <BCM2711 ARM Peripherals> 6.5.2
 */
#define ARM_LOCAL_BASE (0xFF800000U)

#else
#define PBASE 0x107C000000UL
#endif

/* MMU */
#define DEVICE_SIZE         (0x2000000U)
#define ARCH_PHYS_OFFSET    (0)

/* GIC V2 */
#define GIC_V2_DISTRIBUTOR_BASE     (ARM_LOCAL_BASE + 0x00041000)
#define GIC_V2_CPU_INTERFACE_BASE   (ARM_LOCAL_BASE + 0x00042000)

#endif //BASE_H
