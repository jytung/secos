#include <debug.h>
#include <info.h>
#include <pagemem.h>
#include <cr.h>
#include <pagination.h>

#define KRN_idx 0
#define U1_idx 1
#define U2_idx 2
#define SHM_idx 3

void set_pte(pte32_t *ptb, uint32_t privilege, uint32_t index)
{
    for (int i = 0; i < 1024; i++)
    {
        pg_set_entry(&ptb[i], privilege | PG_P, i + (index << 10));
    }
}

void pagination()
{
    /********   Kernel(0 - 0x400000)           ********/
    /********   User1 (0x400000 - 0x800000)    ********/
    /********   User2 (0x800000 - 0xC00000)    ********/
    /********   Shared memory (0xC10000)        ********/
    debug("\nActivate pagination \n");
    pde32_t *kernel_pgd = (pde32_t *)KRN_PGD;
    pde32_t *user1_pgd = (pde32_t *)U1_PGD;
    pde32_t *user2_pgd = (pde32_t *)U2_PGD;

    pte32_t *kernel_ptb = (pte32_t *)KRN_PTB;
    pte32_t *user1_ptb = (pte32_t *)U1_PTB;
    pte32_t *user2_ptb = (pte32_t *)U2_PTB;
    pte32_t *user1_kernel_ptb = (pte32_t *)U1_KRN_PTB;
    pte32_t *user2_kernel_ptb = (pte32_t *)U2_KRN_PTB;
    pte32_t *user1_shared_ptb = (pte32_t *)U1_SHM_PTB;
    pte32_t *user2_shared_ptb = (pte32_t *)U2_SHM_PTB;

    memset((void *)kernel_pgd, 0, PAGE_SIZE);
    memset((void *)user1_pgd, 0, PAGE_SIZE);
    memset((void *)user2_pgd, 0, PAGE_SIZE);

    memset((void *)kernel_ptb, 0, PAGE_SIZE);
    memset((void *)user1_ptb, 0, PAGE_SIZE);
    memset((void *)user1_kernel_ptb, 0, PAGE_SIZE);
    memset((void *)user1_shared_ptb, 0, PAGE_SIZE);
    memset((void *)user2_ptb, 0, PAGE_SIZE);
    memset((void *)user2_kernel_ptb, 0, PAGE_SIZE);
    memset((void *)user2_shared_ptb, 0, PAGE_SIZE);

    pg_set_entry(&kernel_pgd[KRN_idx], PG_RW | PG_KRN | PG_P, page_nr(KRN_PTB));

    pg_set_entry(&user1_pgd[KRN_idx], PG_RW | PG_KRN | PG_P, page_nr(U1_KRN_PTB));
    pg_set_entry(&user1_pgd[U1_idx], PG_RW | PG_KRN | PG_P, page_nr(U1_PTB));
    pg_set_entry(&user1_pgd[SHM_idx], PG_RW | PG_KRN | PG_P, page_nr(U1_SHM_PTB));

    pg_set_entry(&user2_pgd[KRN_idx], PG_RW | PG_KRN | PG_P, page_nr(U2_KRN_PTB));
    pg_set_entry(&user2_pgd[U1_idx], PG_RW | PG_KRN | PG_P, page_nr(U2_PTB));
    pg_set_entry(&user2_pgd[SHM_idx], PG_RW | PG_KRN | PG_P, page_nr(U2_SHM_PTB));

    set_pte(kernel_ptb, PG_RW | PG_KRN, KRN_idx);

    set_pte(user1_ptb, PG_RW | PG_USR, U1_idx);
    set_pte(user1_kernel_ptb, PG_RW | PG_USR, KRN_idx);
    set_pte(user1_shared_ptb, PG_RW | PG_USR, SHM_idx);

    set_pte(user2_ptb, PG_RW | PG_USR, U2_idx);
    set_pte(user2_kernel_ptb, PG_RW | PG_USR, KRN_idx);
    set_pte(user2_shared_ptb, PG_RW | PG_USR, SHM_idx);
    
    pg_set_entry(&user1_ptb[pt32_idx(CPT_USER1_ADDR)], PG_USR | PG_RW | PG_P, page_nr(SHM_ADDR));
    pg_set_entry(&user2_ptb[pt32_idx(CPT_USER2_ADDR)], PG_USR | PG_RW | PG_P, page_nr(SHM_ADDR));

    set_cr3(KRN_PGD);

    //activate pagination
    cr0_reg_t cr0;
    cr0.raw = get_cr0();
    cr0.pg = 1;
    set_cr0(cr0);
}