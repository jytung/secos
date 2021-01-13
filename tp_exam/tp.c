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
        (*cpt_user1)++;
    }
}

void __attribute__((section(".user2"))) user2()
{
    //read in the shared memory
    while (1)
    {
        sys_counter(cpt_user2);
    }
}

void initialise_all_tasks()
{
    pde32_t *kernel_pgd = (pde32_t *)KRN_PGD;
    pde32_t *user1_pgd = (pde32_t *)U1_PGD;
    pde32_t *user2_pgd = (pde32_t *)U2_PGD;

    init_task(krn_task, usr1_task, kernel_pgd);
    init_task(usr1_task, usr2_task, user1_pgd);
    init_task(usr2_task, usr1_task, user2_pgd);
}

void tp()
{
    debug("GDT initialization: \n");
    init_gdt();
    print_gdt();
    debug("TSS initialization: \n");
    init_TSS();
    debug("Activate pagination \n");
    pagination();
    print_gdt();
    debug("Enable interruption \n");
    idt_reg_t idtr;
    get_idtr(idtr);
    debug("Initialise tasks \n");
    initialise_all_tasks();
    debug("Initialise compter to 0 \n");
    *cpt_user1 = 0;
    *cpt_user2 = 0;
    force_interrupts_on();
}
