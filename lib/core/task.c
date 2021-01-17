#include <task.h>

void init_task(task_t *task, task_t *next_task, pde32_t *pgd)
{
    task->next_task = next_task;
    task->pgd = pgd;
}

void init_task_stack(task_t *task, uint32_t *krn_stack, uint32_t *usr_stack, uint32_t eip)
{
    task->krn_stack_ebp = krn_stack;
    task->krn_stack_esp = krn_stack - sizeof(int_ctx_t);
    //initialise the stack content
    int_ctx_t *ctx = (int_ctx_t *)task->krn_stack_esp;
    ctx->cs.raw = gdt_krn_seg_sel(3); //user's code segment
    ctx->eip.raw = eip ;
    ctx->ss.raw = gdt_krn_seg_sel(4); //user's data segment
    ctx->esp.raw = (uint32_t) usr_stack;
    ctx->eflags.raw = gdt_krn_seg_sel(2);
    ctx->gpr.eax.raw = 0x1;
    ctx->gpr.ecx.raw = 0x2;
    ctx->gpr.edx.raw = 0x3;
    ctx->gpr.ebx.raw = 0x4;
    ctx->gpr.esp.raw = (uint32_t) usr_stack;
    ctx->gpr.ebp.raw = (uint32_t) usr_stack;
    ctx->gpr.esi.raw = 0x5;
    ctx->gpr.edi.raw = 0x6;

    memcpy((void *)task->krn_stack_ebp, ctx, sizeof(int_ctx_t));
}
