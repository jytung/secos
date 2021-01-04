/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>
#include <info.h>
#include <segmem.h>
#include <cr.h>
#include <pagemem.h>

extern info_t *info;

#define SIZE_DGR 5
extern info_t *info;
seg_desc_t GDT[SIZE_DGR];

void print_gdt()
{
    gdt_reg_t gdtr;
    get_gdtr(gdtr);
    debug("### GDT ###\n");
    debug("size of GDT: %d\n", gdtr.limit);
    debug("ss: %d\t", get_ss());
    debug("ds: %d\t", get_ds());
    debug("es: %d\t", get_es());
    debug("fs: %d\t", get_fs());
    debug("gs: %d\n", get_gs());
    int n = (gdtr.limit + 1) / sizeof(seg_desc_t);
    //debug("n: %d\n",n);
    for (int i = 0; i < n; i++)
    {
        seg_desc_t *desc = &gdtr.desc[i];
        uint64_t base = desc->base_1 + (desc->base_2 << 16) + (desc->base_3 << 24);
        uint16_t limit = desc->limit_1 + (desc->limit_2 << 16);
        debug("----------------GDT[%d]--------------------\n ", i);
        debug("limit: %d \t", limit);
        debug("base: %x \t", base);
        debug("dpl: %d \t", desc->dpl);
        debug("type: %d \n ", desc->type);
    }
}

void set_each_descriptor(seg_desc_t *seg, uint8_t type, uint8_t dpl, uint32_t base, uint32_t limit)
{
    seg->type = type;
    seg->dpl = dpl;
    seg->base_1 = base & 0xffff;
    seg->base_2 = (base >> 16) & 0xffff;
    seg->base_3 = (base >> 24) & 0xffff;
    seg->limit_1 = limit & 0xffff;
    seg->limit_2 = (limit >> 16) & 0xf;
    seg->p = 1;
    seg->avl = 1;
    seg->d = 1;
    seg->g = 1;
    seg->l = 0;
    seg->s = 1;
}

void init_gdt()
{

    GDT[0].raw = 0ULL;
    set_each_descriptor(&GDT[1], SEG_DESC_CODE_XR, SEG_SEL_KRN, 0, 0xffffffff);
    set_each_descriptor(&GDT[2], SEG_DESC_DATA_RW, SEG_SEL_KRN, 0, 0xffffffff);

    set_each_descriptor(&GDT[3], SEG_DESC_CODE_XR, SEG_SEL_USR, 0, 0xffffffff);
    set_each_descriptor(&GDT[4], SEG_DESC_DATA_RW, SEG_SEL_USR, 0, 0xffffffff);

    gdt_reg_t gdtr;
    gdtr.limit = SIZE_DGR * sizeof(seg_desc_t) - 1;
    gdtr.desc = GDT;
    set_gdtr(gdtr);
    //Ring 0
    set_cs(gdt_krn_seg_sel(1));
    set_ds(gdt_krn_seg_sel(2));
    set_ss(gdt_krn_seg_sel(2));
    set_fs(gdt_krn_seg_sel(2));
    set_es(gdt_krn_seg_sel(2));
    set_gs(gdt_krn_seg_sel(2));
    //Ring3
    //set_cs(gdt_krn_seg_sel(3));
    set_ds(gdt_usr_seg_sel(4));
    set_es(gdt_usr_seg_sel(4));
    set_fs(gdt_usr_seg_sel(4));
    set_gs(gdt_usr_seg_sel(4));
}

void sys_counter(uint32_t *counter)
{
    debug("%d", &counter);
}

void userland(){
    debug("hello from userland");
    //asm volatile ("int $32");
}

void __regparm__(1) handler()
{
   debug("into the handler ");
}

void handler_32(){
    //je fais quoi a chaque tick d'horloge??
    //passer a la tache 1 et tache 2
    debug("hello handler_32");
    asm volatile (
      "leave ; pusha        \n"
      "call handler         \n"
      "popa ;iret"
    );
}

void __attribute__((section(".user1"))) user1(pde32_t *pgd)
{
    //write in the shared memory
    int cpt = 0;
    while (1)
    {
        set_cr3((uint32_t)pgd);
        //write
        int* location= (int*)0x600000;
        memset(location, cpt, sizeof(int));
        cpt++;
        //interruption
        //asm volatile("int $32");   
    }     
}


void __attribute__((section(".user2"))) user2(pde32_t *pgd)
{
    //read in the shared memory
    while (1)
    {
        uint32_t* location= (uint32_t*)0x600000;
        set_cr3((uint32_t)pgd);
        sys_counter(location);
        //interruption

    }
}

void activate_pagination()
{
    cr0_reg_t cr0;
    cr0.raw = get_cr0();
    cr0.pg = 1;
    set_cr0(cr0);
}

void tp()
{
    //initialisation GDT
    init_gdt();
    debug("GDT initialization: \n");
    print_gdt();

    //allocate Kernel PGD and PTB
    pde32_t *kernel_pgd = (pde32_t *)0x390000;
    pte32_t *kernel_ptb = (pte32_t *)0x391000;
    //identity mapping of kernel (0 - 0x400000)
    for (int i = 0; i < 1024; i++)
    {
        pg_set_entry(&kernel_ptb[i], PG_RW | PG_KRN, i);
    }
    memset((void *)kernel_pgd, 0, PAGE_SIZE);
    pg_set_entry(&kernel_pgd[0], PG_RW | PG_KRN, page_nr(kernel_ptb));

    set_cr3((uint32_t)kernel_pgd);

    //allocate user1's PGD (0x400000 - 0x800000)
    pde32_t *user1_pgd = (pde32_t *)0x400000;
    pg_set_large_entry(user1_pgd, PG_RW | PG_USR, 0);
    memset((void *)user1_pgd, 0, PG_4M_SIZE);

    //allocate user2's PGD (0x800000 - 0x1200000)
    pde32_t *user2_pgd = (pde32_t *)0x800000;
    pg_set_large_entry(user2_pgd, PG_RO | PG_USR, 0);
    memset((void *)user1_pgd, 0, PG_4M_SIZE);

    //allocate shared memory PGD for user1
    pde32_t *shared_pgd_1 = (pde32_t *)0x1200000;
    //pg_set_large_entry(shared_pgd_1, PG_RW | PG_USR, 0);
    //memset((void *)user1_pgd, 0, PG_4M_SIZE);
    pte32_t *shared_ptb_1 = (pte32_t *)0x1201000;
    pg_set_entry(&shared_pgd_1[0], PG_RO | PG_USR, 4000);
    memset((void *)shared_pgd_1, 0, PAGE_SIZE);
    pg_set_entry(&shared_pgd_1[0], PG_RW | PG_KRN, page_nr(shared_ptb_1));

    //allocate shared memory PGD for user2
    pde32_t *shared_pgd_2 = (pde32_t *)0x1200000;
    pte32_t *shared_ptb_2 = (pte32_t *)0x1201000;
    pg_set_entry(&shared_pgd_2[0], PG_RO | PG_USR, 4000);
    memset((void *)shared_pgd_2, 0, PAGE_SIZE);
    pg_set_entry(&shared_pgd_2[0], PG_RW | PG_KRN, page_nr(shared_ptb_2));
    activate_pagination();

    idt_reg_t idtr;
    get_idtr(idtr);
    int_desc_t *dsc = &idtr.desc[32];
    dsc->dpl = 3;

    dsc->offset_1 = (uint16_t)((uint32_t)handler_32);
    dsc->offset_2 = (uint16_t)(((uint32_t)handler_32) >> 16);
    debug("before going to userland");

    /*go to userland to trigger int 32*/
    asm volatile (
      "push %0 \n" // ss
      "push %%ebp \n" // esp
      "pushf   \n" // eflags
      "push %1 \n" // cs
      "push %2 \n" // eip
      "iret"
      ::
       "i"(gdt_usr_seg_sel(4)),
       "i"(gdt_usr_seg_sel(3)),
       "r"(&userland)
      );
}
