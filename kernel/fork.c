#include "sched.h"
#include "memory.h"
#include "asm/system.h"

#define __init_task_data __attribute__((__section__(".data.init_task")))

/* init task. */
union task_union init_task_union __init_task_data = {INIT_TASK(init_task_union.task)};
unsigned long total_forks;
/* All task pcb array. */
struct task_struct *g_task[NR_TASKS] = {&init_task_union.task,};

#define SET_LINKS(p) \
    do { \
        (p)->next_task = &init_task; \
        (p)->prev_task = init_task.prev_task; \
        init_task.prev_task->next_task = (p); \
        init_task.prev_task = (p); \
    } while (0)

static int find_empty_task(void)
{
    int i;

    for (i = 0 ; i < NR_TASKS; i++) {
        if (!g_task[i])
            return i;
    }

    return -1;
}

static struct pt_regs *task_pt_regs(struct task_struct *tsk)
{
    unsigned long p;
    p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
    return (struct pt_regs *)p;
}

static int copy_thread(unsigned long clone_flags, struct task_struct *p,
        unsigned long fn, unsigned long arg)
{
    struct pt_regs *childregs;

    childregs = task_pt_regs(p);
    memset(childregs, 0, sizeof(struct pt_regs));
    memset(&p->cpu_context, 0, sizeof(struct cpu_context));

    if (clone_flags & PF_KTHREAD) {
        childregs->pstate = PSR_MODE_EL1h;
        p->cpu_context.x19 = fn;
        p->cpu_context.x20 = arg;
    }

    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->cpu_context.sp = (unsigned long)childregs;

    return 0;
}

/* fork a new task. */
int do_fork(unsigned long clone_flags, unsigned long fn, unsigned long arg)
{
    struct task_struct *p;
    int pid;

    p = (struct task_struct *)get_free_page();
    if (!p)
        goto error;

    pid = find_empty_task();
    if (pid < 0)
        goto error;

    if (copy_thread(clone_flags, p, fn, arg))
        goto error;

    p->state = TASK_RUNNING;
    p->pid = pid;
    p->counter = (current->counter + 1) >> 1;
    current->counter >>= 1;
    p->need_resched = 0;
    p->preempt_count = 0;
    p->priority = 2;
    total_forks++;
    g_task[pid] = p;
    SET_LINKS(p);
    wake_up_process(p);

    return pid;
error:
    return -1;
}
