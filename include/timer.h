#ifndef TIMER_H
#define TIMER_H

void timer_init();
void handle_timer_irq(void);

void system_timer_init(void);
void handle_stimer_irq(void);

#endif //TIMER_H
