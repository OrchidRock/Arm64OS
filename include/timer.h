#ifndef TIMER_H
#define TIMER_H

#define MSEC_PER_SEC	1000L
#define USEC_PER_MSEC	1000L
#define NSEC_PER_USEC	1000L
#define NSEC_PER_MSEC	1000000L
#define USEC_PER_SEC	1000000L
#define NSEC_PER_SEC	1000000000L

#define CONFIG_HZ_250 1
#define CONFIG_HZ 250
#define HZ CONFIG_HZ	/* Internal kernel timer frequency */

#endif //TIMER_H
