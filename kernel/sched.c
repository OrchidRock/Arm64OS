//
// Created by mimose on 25-3-23.
//
#include "sched.h"
#include "list.h"
#include "irq.h"
#include "printk.h"

struct run_queue g_ready_queue;
#define preempt_count() (current->preempt_count)
#define in_atomic_preempt_off() (preempt_count() != 1)

struct task_struct *pick_next_task(struct run_queue *rq,
        struct task_struct *prev)
{
    const struct sched_class *class = &nop_sched_class;
    return class->pick_next_task(rq, prev);
}
static void dequeue_task(struct run_queue *rq, struct task_struct *p)
{
    const struct sched_class *class = &nop_sched_class;
    return class->dequeue_task(rq, p);
}

void enqueue_task(struct run_queue *rq, struct task_struct *p)
{
    const struct sched_class *class = &nop_sched_class;
    return class->enqueue_task(rq, p);
}

void task_tick(struct run_queue *rq, struct task_struct *p)
{
    const struct sched_class *class = &nop_sched_class;
    return class->task_tick(rq, p);
}

void tick_handle_periodic(void)
{
    struct run_queue *rq = &g_ready_queue;
    task_tick(rq, current);
}

static inline void preempt_disable(void)
{
    current->preempt_count++;
}

static inline void preempt_enable(void)
{
    current->preempt_count--;
}

static void schedule_debug(struct task_struct *p)
{
    if (in_atomic_preempt_off())
        printk("BUG: scheduling while atomic: %d, 0x%x\n", p->pid, preempt_count());
}

static inline void clear_task_resched(struct task_struct *p)
{
    p->need_resched = 0;
}

struct task_struct * switch_to(struct task_struct *prev, struct task_struct *next)
{
    if (prev == next)
        return NULL;

    return cpu_switch_to(prev, next);
}

void schedule_tail(struct task_struct *prev)
{
    /* 打开中断 */
    raw_local_irq_enable();
}

static void __schedule(void)
{
    struct task_struct *prev, *next, *last;
    struct run_queue *rq = &g_ready_queue;

    prev = current;

    /* check whither is runing within interrupt context. */
    schedule_debug(prev);

    /* disable irq*/
    raw_local_irq_disable();

    if (prev->state)
        dequeue_task(rq, prev);

    next = pick_next_task(rq, prev);
    clear_task_resched(prev);
    if (next != prev) {
        last = switch_to(prev, next);
        /*
         * switch_to函数是用来切换prev进程到next进程。
         * switch_to函数执行完成之后，已经切换到next
         * 进程,整个栈和时空都发生变化了,不能使用这
         * 里的prev变量来表示prev进程，只能通过aarch64
         * 的x0寄存器来获取prev进程的task_struct。
         *
         * 在switch_to函数使用x0寄存器来传递prev进程
         * task_struct,返回值也是通过x0寄存器，因此
         * 这里last变量表示prev进程的task_struct
         */
        rq->nr_switches++;
        rq->curr = current;
    }

    /* 由next进程来收拾prev进程的现场 */
    schedule_tail(last);
}

void schedule(void)
{
    /* close preempt. */
    preempt_disable();
    __schedule();
    preempt_enable();
}

void preempt_schedule_irq(void)
{
    /* this must be preemptible now*/
    if (preempt_count())
        printk("BUG: %s incorrect preempt count: 0x%x\n",
                __func__, preempt_count());

    /* 关闭抢占，以免嵌套发生调度抢占*/
    preempt_disable();
    /*
     * 这里打开中断，处理高优先级中断，
     * 中断比抢占调度优先级。
     *
     * 若这里发生中断，中断返回后，前面关闭抢占
     * 不会嵌套发生抢占调度
     */
    raw_local_irq_enable();
    __schedule();
    raw_local_irq_disable();
    preempt_enable();
}

void wake_up_process(struct task_struct *p)
{
    struct run_queue *rq = &g_ready_queue;
    p->state = TASK_RUNNING;
    enqueue_task(rq, p);
}

void sched_init(void)
{
    struct run_queue *rq = &g_ready_queue;
    INIT_LIST_HEAD(&rq->rq_head);
    rq->nr_running = 0;
    rq->nr_switches = 0;
    rq->curr = NULL;
}