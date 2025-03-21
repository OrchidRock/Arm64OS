#ifndef BASE_H
#define BASE_H


#ifndef RPI5_BCM2712
#define PBASE (0xFE000000U)
#else
#define PBASE 0x107C000000UL
#endif

#endif //BASE_H
