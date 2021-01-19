#include <segmem.h>
#include <debug.h>
#include <info.h>

#define SIZE_GDT 6
seg_desc_t GDT[SIZE_GDT];
tss_t TSS;

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

void init_TSS()
{
    debug("\nTSS initialization \n");
    offset_t base = (offset_t)&TSS;
    uint32_t limit = sizeof(TSS) - 1;

    TSS.s0.ss = gdt_krn_seg_sel(2);
    TSS.s0.esp = get_ebp();
    set_each_descriptor(&GDT[5], SEG_DESC_SYS_TSS_AVL_32, SEG_SEL_KRN, base, limit);
    GDT[5].s = 0;
    set_tr(gdt_krn_seg_sel(5));
}

void init_gdt()
{
    debug("\nGDT initialization: \n");
    GDT[0].raw = 0ULL;
    gdt_reg_t gdtr;
    gdtr.limit = SIZE_GDT * sizeof(seg_desc_t) - 1;
    gdtr.desc = GDT;
    set_gdtr(gdtr);

    //Ring 0
    set_each_descriptor(&GDT[1], SEG_DESC_CODE_XR, SEG_SEL_KRN, 0, 0xffffffff);
    set_each_descriptor(&GDT[2], SEG_DESC_DATA_RW, SEG_SEL_KRN, 0, 0xffffffff);
    set_cs(gdt_krn_seg_sel(1));
    set_ds(gdt_krn_seg_sel(2));
    set_ss(gdt_krn_seg_sel(2));
    set_fs(gdt_krn_seg_sel(2));
    set_es(gdt_krn_seg_sel(2));
    set_gs(gdt_krn_seg_sel(2));

    //Ring3
    set_each_descriptor(&GDT[3], SEG_DESC_CODE_XR, SEG_SEL_USR, 0, 0xffffffff);
    set_each_descriptor(&GDT[4], SEG_DESC_DATA_RW, SEG_SEL_USR, 0, 0xffffffff);
    set_ds(gdt_usr_seg_sel(4));
    set_es(gdt_usr_seg_sel(4));
    set_fs(gdt_usr_seg_sel(4));
    set_gs(gdt_usr_seg_sel(4));
    print_gdt();
}