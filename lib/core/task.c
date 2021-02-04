#include <task.h>
#include <pic.h>
#include <gdt.h>

extern void resume_from_intr();

void init_task(task_t *task, task_t *next_task, pde32_t *pgd)
{
    task->next_task = next_task;
    task->pgd = pgd;
}

void init_task_stack(task_t *task, uint32_t *krn_stack_ebp, uint32_t *usr_stack_ebp, uint32_t eip)
{
    task->krn_stack_ebp = krn_stack_ebp;
    task->krn_stack_esp = task->krn_stack_ebp - sizeof(int_ctx_t);

    //initialise the stack content
    int_ctx_t *ctx = (int_ctx_t *)task->krn_stack_esp;
    ctx->nr.raw=PIC_IRQ_NR;
    ctx->err.raw = -1;
    ctx->cs.raw = user_code_segment; //user's code segment
    ctx->eip.raw = eip ; 
    ctx->ss.raw = user_data_segment; //user's data segment
    ctx->esp.raw = (uint32_t) usr_stack_ebp ;
    ctx->eflags.raw  = 0x200; // 0x200 EFLAGS_IF
    ctx->gpr.eax.raw = 0x1;
    ctx->gpr.ecx.raw = 0x2;
    ctx->gpr.edx.raw = 0x3;
    ctx->gpr.ebx.raw = 0x4;
    ctx->gpr.esp.raw = 0x5;
    ctx->gpr.ebp.raw = (uint32_t) usr_stack_ebp;
    ctx->gpr.esi.raw = 0x7;
    ctx->gpr.edi.raw = 0x8;
         
    *(--task->krn_stack_esp)= (uint32_t) resume_from_intr;
    *(--task->krn_stack_esp)= 0xbeef; //fausse ebp 
}

void print_task(task_t *task){
    debug("\n print task \n");
    debug(" ebp = %x\n",task->krn_stack_ebp);
    debug(" esp = %x\n",task->krn_stack_esp);
    debug(" pgd = %x\n",task->pgd);
    debug(" next_task = %x\n",task->next_task);
}