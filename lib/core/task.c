#include <task.h>
extern void resume_from_intr();
void init_task(task_t *task, task_t *next_task, pde32_t *pgd)
{
    task->next_task = next_task;
    task->pgd = pgd;
}

void init_task_stack(task_t *task, uint32_t *krn_stack, uint32_t *usr_stack, uint32_t eip)
{
    task->krn_stack_ebp = krn_stack;
    task->krn_stack_esp = task->krn_stack_ebp - sizeof(int_ctx_t);
    //initialise the stack content
    int_ctx_t *ctx = (int_ctx_t *)task->krn_stack_esp;
    ctx->cs.raw = gdt_krn_seg_sel(3); //user's code segment
    ctx->eip.raw = eip ;
    ctx->ss.raw = gdt_krn_seg_sel(4); //user's data segment
    ctx->esp.raw = (uint32_t) usr_stack;
    ctx->eflags.raw = gdt_krn_seg_sel(2);
    ctx->gpr.eax.raw = 0x0;
    ctx->gpr.ecx.raw = 0x0;
    ctx->gpr.edx.raw = 0x0;
    ctx->gpr.ebx.raw = 0x0;
    ctx->gpr.esp.raw = 0x0;
    ctx->gpr.ebp.raw = (uint32_t) usr_stack;
    ctx->gpr.esi.raw = 0x0;
    ctx->gpr.edi.raw = 0x0;

    memcpy((void *)task->krn_stack_ebp, ctx, sizeof(int_ctx_t));

    *(--task->krn_stack_esp)= (offset_t) resume_from_intr;
    *(--task->krn_stack_esp)= 0xbeef; //ebp 
}
