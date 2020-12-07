/* GPLv2 (c) Airbus */

#include <debug.h>
#include <info.h>
#include <cr.h>
#include <pagemem.h>
extern info_t *info;


void print_cr3(){
    //Question 1
    uint32_t cr3= get_cr3();
    debug("CR3 = %x\n", cr3);
}

void activer_pagination(){
    //q3
    cr0_reg_t cr0 ;
    cr0.raw = get_cr0();
    cr0.pg=1;
    set_cr0(cr0); 
    // reboot infini?? car on n'a pas encore faire du identity mapping
}

void tp(){
    //q2
    pde32_t* pgd = (pde32_t*)0x600000 ;
    set_cr3(pgd);
    print_cr3();

    //q4
    //initialisation de page table
    pte32_t* ptb = (pte32_t*)0x601000 ;
    //identity mapping - ptb peut contient 2^10 de l'address donc 1024
    for(int i =0;i<1024;i++){
        pg_set_entry(&ptb[i], PG_RW|PG_KRN, i);
    }
    memset((void*)pgd,0,PAGE_SIZE);
    pg_set_entry(&pgd[0],PG_RW|PG_KRN,page_nr(ptb));
    
    debug("PTB[0] =%p\n",ptb[0].raw);
    
    //6
    pte32_t* ptb2 = (pte32_t*)0x602000 ;
    //identity mapping 
    //maintenant on peut mapper jusqu'a 8M
    for(int i =0;i<1024;i++){
        pg_set_entry(&ptb2[i], PG_RW|PG_KRN, i+1024);
    }
    pg_set_entry(&pgd[1],PG_RW|PG_KRN,page_nr(ptb2));
    
    set_cr3((uint32_t)pgd);
    activer_pagination();

    debug("PTB[1] =%p\n",ptb[1].raw);
    
    //7
    pte32_t* ptb3 = (pte32_t*)0x603000 ;
    uint32_t *target = (uint32_t*)0xc000000;
    int idx = pd32_idx(target);

    memset((void*)ptb3,0,PAGE_SIZE);
    for(int i =0;i<1024;i++){
        pg_set_entry(&ptb3[i], PG_RW|PG_KRN, i+1024*2);
    }
    // pg_set_entry(&ptb3[idx],PG_RW|PG_KRN,page_nr(pgd));
    pg_set_entry(&pgd[idx],PG_RW|PG_KRN,page_nr(ptb3));

    debug("PGD[0] =%x  | target= %p\n",pgd[0].addr,*target);

    //8 - memoire partargee
    char *p1= (char*) 0x700000;
    char *p2= (char*) 0x7ff000;

    idx= pd32_idx(p1);
    pg_set_entry(&ptb2[idx],PG_RW|PG_KRN,2);

    idx= pd32_idx(p2);
    pg_set_entry(&ptb2[idx],PG_RW|PG_KRN,2);
    debug("%p=%x | %p=%x",p1,*p1,p2,*p2);
}

