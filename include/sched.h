#ifndef SCHED_H
#define SCHED_H

#include "asm/processor.h"
#include "page.h"

/* kernel statck: one page (4K). */
#define THREAD_SIZE  (1 * PAGE_SIZE)
#define THREAD_START_SP  (THREAD_SIZE - 8)
#define NR_TASKS 128

enum task_state {
    TASK_RUNNING           = 0,
    TASK_INTERRUPTIBLE     = 1,
    TASK_UNINTERRUPTIBLE   = 2,
    TASK_ZOMBIE            = 3,
    TASK_STOPPED           = 4,
};

enum task_flags {
    PF_KTHREAD = 1 << 0,
};

/* process control block. */
struct task_struct {
    enum task_state state;
    enum task_flags flags;
    long count;
    int priority;
    int pid;
    struct cpu_context cpu_context;
};

/* kernel stack. */
/* task_struct located at the bottom of the kernel stack */
union task_union {
    struct task_struct task;
    unsigned long stack[THREAD_SIZE/sizeof(long)];
};

/* init task (pid: 0) */
#define INIT_TASK(task) \
{                      \
.state = 0,     \
.priority = 1,   \
.flags = PF_KTHREAD,   \
.pid = 0,     \
}

extern struct task_struct *g_task[];

extern void ret_from_fork(void);
int do_fork(unsigned long clone_flags, unsigned long fn, unsigned long arg);
void switch_to(struct task_struct *next);
extern struct task_struct *cpu_switch_to(struct task_struct *prev,
                     struct task_struct *next);

#endif //SCHED_H
