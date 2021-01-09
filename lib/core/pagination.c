#include <debug.h>
#include <info.h>
#include <pagemem.h>
#include <cr.h>
#include <pagination.h>

#define KRN_idx 0
#define U1_idx 1
#define U2_idx 2
#define SHM_idx 3

//allocate PGD and PTB
pde32_t *kernel_pgd = (pde32_t *)0x390000;
pde32_t *user1_pgd = (pde32_t *)0x391000;
pde32_t *user2_pgd = (pde32_t *)0x392000;

pte32_t *kernel_ptb = (pte32_t *)0x393000;
pte32_t *user1_ptb = (pte32_t *)0x394000;
pte32_t *user2_ptb = (pte32_t *)0x395000;
pte32_t *user1_kernel_ptb = (pte32_t *)0x396000;
pte32_t *user2_kernel_ptb = (pte32_t *)0x397000;
pte32_t *user1_shared_ptb = (pte32_t *)0x398000;
pte32_t *user2_shared_ptb = (pte32_t *)0x399000;

void set_pte(pte32_t *ptb, uint32_t privilege, uint32_t index)
{
    for (int i = 0; i < 1024; i++)
    {
        pg_set_entry(&ptb[i], privilege, i + (index << 10));
    }
}

void pagination()
{

    /********Kernel(0 - 0x400000) ********/
    memset((void *)kernel_pgd, 0, PAGE_SIZE);
    set_pte(kernel_ptb, PG_RW | PG_KRN, KRN_idx);
    pg_set_entry(&kernel_pgd[KRN_idx], PG_RW | PG_KRN, page_nr(kernel_ptb));

    /********User1 (0x400000 - 0x800000) ********/
    memset((void *)user1_ptb, 0, PAGE_SIZE);
    memset((void *)user1_kernel_ptb, 0, PAGE_SIZE);
    memset((void *)user1_shared_ptb, 0, PAGE_SIZE);
    set_pte(user1_ptb, PG_RW | PG_USR, U1_idx);
    set_pte(user1_kernel_ptb, PG_RW | PG_USR, KRN_idx);
    pg_set_entry(&user1_shared_ptb[SHM_idx], PG_RW | PG_USR, (SHM_idx << 10));
    pg_set_entry(&user1_pgd[KRN_idx], PG_RW | PG_KRN, page_nr(user1_kernel_ptb));
    pg_set_entry(&user1_pgd[U1_idx], PG_RW | PG_KRN, page_nr(user1_ptb));
    

    /********User2 (0x800000 - 0x1200000) ********/
    memset((void *)user2_ptb, 0, PAGE_SIZE);
    memset((void *)user2_kernel_ptb, 0, PAGE_SIZE);
    memset((void *)user2_shared_ptb, 0, PAGE_SIZE);
    set_pte(user2_ptb, PG_RW | PG_USR, U1_idx);
    set_pte(user2_kernel_ptb, PG_RW | PG_USR, KRN_idx);
    pg_set_entry(&user2_shared_ptb[SHM_idx], PG_RW | PG_USR, (SHM_idx << 10));
    pg_set_entry(&user2_pgd[KRN_idx], PG_RO | PG_KRN, page_nr(user2_kernel_ptb));
    pg_set_entry(&user2_pgd[U1_idx], PG_RO | PG_KRN, page_nr(user2_ptb));
    

    /********Shared memory ********/
    pg_set_entry(&user1_pgd[SHM_idx], PG_RW | PG_KRN, page_nr(user1_shared_ptb));
    pg_set_entry(&user2_pgd[SHM_idx], PG_RO | PG_KRN, page_nr(user2_shared_ptb));

    pg_set_entry(&user1_shared_ptb[pt32_idx(CPT_USER1_ADDR)], PG_USR | PG_RW, page_nr(SHM_ADDR));
    pg_set_entry(&user2_shared_ptb[pt32_idx(CPT_USER2_ADDR)], PG_USR | PG_RO, page_nr(SHM_ADDR));

    set_cr3((uint32_t)kernel_pgd);

    //activate pagination
    cr0_reg_t cr0;
    cr0.raw = get_cr0();
    cr0.pg = 1;
    set_cr0(cr0);
}