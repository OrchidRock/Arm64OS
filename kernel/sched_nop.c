//
// Created by mimose on 25-3-23.
//

#include "sched.h"
#include "list.h"
#include "printk.h"

static void dequeue_task_nop(struct run_queue *rq, struct task_struct *p)
{
    rq->nr_running--;
    list_del(&p->run_list);
}

static void enqueue_task_nop(struct run_queue *rq, struct task_struct *p)
{
    list_add(&p->run_list, &rq->rq_head);
    rq->nr_running++;
}

static void task_tick_nop(struct run_queue *rq, struct task_struct *p)
{
    if (--p->counter <= 0) {
        p->counter = 0;
        p->need_resched = 1;
        printk("pid %d need_resched\n", p->pid);
    }
}

static inline int goodness(struct task_struct *p)
{
    return p->counter;
}

static void reset_score(void)
{
    struct task_struct *p;

    for_each_task(p) {
        p->counter = DEF_COUNTER + p->priority;
        //printk("%s, pid=%d, count=%d\n", __func__, p->pid, p->counter);
    }
}

static struct task_struct *pick_next_task_nop(struct run_queue *rq,
        struct task_struct *prev)
{
    struct task_struct *p, *next;
    struct list_head *tmp;
    int weight;
    int c;

repeat:
    c = -1000;
    list_for_each(tmp, &rq->rq_head) {
        p = list_entry(tmp, struct task_struct, run_list);
        weight = goodness(p);
        if (weight > c) {
            c = weight;
            next = p;
        }
    }

    if (!c) {
        reset_score();
        goto repeat;
    }

    //printk("%s: pick next thread (pid %d)\n", __func__, next->pid);

    return next;
}
const struct sched_class nop_sched_class = {
    .next = NULL,
    .dequeue_task = dequeue_task_nop,
    .enqueue_task = enqueue_task_nop,
    .task_tick = task_tick_nop,
    .pick_next_task = pick_next_task_nop,
};