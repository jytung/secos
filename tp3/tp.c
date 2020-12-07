/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h> 
#include <types.h> 

#define SIZE_DGR 5
extern info_t *info;
seg_desc_t GDT [SIZE_DGR];

void print_gdt(){
    gdt_reg_t gdtr;
    get_gdtr(gdtr);
    debug("### GDT ###\n");
    debug("size of GDT: %d\n",gdtr.limit);
    debug("ss: %d\t",get_ss());
    debug("ds: %d\t",get_ds());
    debug("es: %d\t",get_es());
    debug("fs: %d\t",get_fs());
    debug("gs: %d\n",get_gs());
    int n= (gdtr.limit+1)/sizeof(seg_desc_t);
    //debug("n: %d\n",n);
    for(int i=0;i<n;i++){
        seg_desc_t *desc = &gdtr.desc[i];
        uint64_t base= desc->base_1 + (desc->base_2<<16) + (desc->base_3<<24);
        uint16_t limit= desc->limit_1 + (desc->limit_2<<16);
        debug("----------------GDT[%d]--------------------\n ",i);
        debug("limit: %d \t",limit);
        debug("base: %x \t",base);
        debug("dpl: %d \t",desc->dpl);
        debug("type: %d \n ",desc->type);
    }
}

void set_each_descriptor(seg_desc_t* seg, uint8_t type, uint8_t dpl,uint32_t base, uint32_t limit){
    seg->type=type;
    seg->dpl=dpl;
    seg->base_1=base & 0xffff;
    seg->base_2=(base>>16) & 0xffff;
    seg->base_3=(base>>24) & 0xffff;
    seg->limit_1=limit & 0xffff;
    seg->limit_2=(limit>>16) & 0xf;
    seg->p=1;
    seg->avl=1;
    seg->d=1;
    seg->g=1;
    seg->l=0;
    seg->s=1;
}

void init_gdt(){
    
    GDT[0].raw = 0ULL;
    set_each_descriptor(&GDT[1],SEG_DESC_CODE_XR,SEG_SEL_KRN,0, 0xffffffff);
    set_each_descriptor(&GDT[2],SEG_DESC_DATA_RW,SEG_SEL_KRN,0, 0xffffffff);

    set_each_descriptor(&GDT[3],SEG_DESC_CODE_XR,SEG_SEL_USR,0, 0xffffffff);
    set_each_descriptor(&GDT[4],SEG_DESC_DATA_RW,SEG_SEL_USR,0, 0xffffffff);
    
    gdt_reg_t gdtr;
    gdtr.limit=SIZE_DGR*sizeof(seg_desc_t)-1;
    gdtr.desc= GDT;
    set_gdtr(gdtr);

    set_cs(gdt_krn_seg_sel(1));
    set_ds(gdt_krn_seg_sel(2));
    set_ss(gdt_krn_seg_sel(2));
    set_fs(gdt_krn_seg_sel(2));
    set_es(gdt_krn_seg_sel(2));
    set_gs(gdt_krn_seg_sel(2)); 
}

void userland()
{
    debug("Enter userland\n");
    asm volatile ("mov %eax, %cr0");
}

void chargement_de_segments(){
    //question 3.1
    set_ds(gdt_usr_seg_sel(4));
    set_es(gdt_usr_seg_sel(4));
    set_fs(gdt_usr_seg_sel(4));
    set_gs(gdt_usr_seg_sel(4));

    //question 3.2 
    //set_ss(gdt_usr_seg_sel(4));
    //ERROR general protection PK???
    //dpl>cpl => error
}

void farjump_to_userland(){
    //question 3.3
    //set_cs(gdt_usr_seg_sel(3));
    fptr32_t fptr;
    fptr.offset=(uint32_t)userland;
    fptr.segment=gdt_usr_seg_sel(3);
    farjump(fptr);

    //ERROR? WHY? parce que un user de ring 3 n'a pas de droit de modifier le registre cr0
}

void go_to_userland(){
    asm volatile(   "push %0;"      //push ss
                    "push %%esp;"   //push esp
                    "pushf;"        //push flags
                    "push %1;"      //push cs
                    "push %2;"      //push EIP
                    "iret;"
                    ::"i"(gdt_usr_seg_sel(4)), "i"(gdt_usr_seg_sel(3)), "r"(&userland)
                );
}

void tp()
{
    init_gdt();
    debug("######after initialization######\n");
    print_gdt();
    chargement_de_segments();
    debug("######after chargement_de_segments######\n");
    print_gdt();
    debug("######after farjump######\n");
    //farjump_to_userland();
    go_to_userland();
    print_gdt();
}
