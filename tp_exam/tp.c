/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>
#include <segmem.h>
#include <cr.h>
#include <asm.h>
#include <gdt.h>
#include <pagination.h>
#include <task.h>

extern info_t *info;
extern task_t *current_task;
uint32_t *cpt_user1 = (uint32_t *)CPT_USER1_ADDR; 
uint32_t *cpt_user2 = (uint32_t *)CPT_USER2_ADDR; 

task_t *krn_task;
task_t *usr1_task;
task_t *usr2_task;

void sys_counter(uint32_t *cpt)
{
    asm volatile("mov %0, %%esi \n"
                 "int $80" ::"m"(cpt));
}

void __attribute__((section(".user1"))) user1()
{
    //write in the shared memory
    while (1)
    {
        //(*cpt_user1)++;
        debug("user1");
    }
    
   
}

void __attribute__((section(".user2"))) user2()
{
    //read in the shared memory
    while (1)
    {
        //sys_counter(cpt_user2);
        debug("user2");
    }
    
}

void initialise_all_tasks()
{
    debug("\nInitialise tasks \n");
    pde32_t *kernel_pgd = (pde32_t *)KRN_PGD;
    pde32_t *user1_pgd = (pde32_t *)U1_PGD;
    pde32_t *user2_pgd = (pde32_t *)U2_PGD;

    init_task(krn_task, usr1_task, kernel_pgd);
    init_task(usr1_task, usr2_task, user1_pgd);
    init_task(usr2_task, usr1_task, user2_pgd);
    init_task_stack(usr1_task, (uint32_t*)KERNEL_T1_STACK, (uint32_t*)USER1_STACK,(uint32_t) &user1);
    init_task_stack(usr2_task, (uint32_t*)KERNEL_T2_STACK, (uint32_t*)USER2_STACK,(uint32_t) &user2);

    current_task= krn_task;
}

void tp()
{
    
    init_gdt();
    init_TSS();

    debug("\nInitialise compter to 0 \n");
    *cpt_user1 = 0;
    *cpt_user2 = 0;

    pagination();
    debug("\nEnable interruption \n");
    idt_reg_t idtr;
    get_idtr(idtr);

    initialise_all_tasks();
    
    force_interrupts_on();

    while(1);
}
