#ifndef __TASK_H__
#define __TASK_H__

#include <pagemem.h>
#include <pagination.h>
#include <intr.h>
#include <gpr.h>

typedef struct task
{
    pde32_t *           pgd;
    struct task *       next_task;
    uint32_t*           krn_stack_ebp;
    uint32_t*           krn_stack_esp;
} task_t;

void init_task(task_t * task, task_t *next_task, pde32_t * pgd);
void init_task_stack(task_t *task, uint32_t *krn_stack, uint32_t *usr_stack, uint32_t eip);

#endif