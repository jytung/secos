#ifndef __GDT_H__
#define __GDT_H__

#include <segmem.h>
#include <debug.h>
#include <info.h>

#define SIZE_GDT 6
#define krn_code_segment    gdt_krn_seg_sel(1)
#define krn_data_segment    gdt_krn_seg_sel(2)
#define user_code_segment   gdt_usr_seg_sel(3)
#define user_data_segment   gdt_usr_seg_sel(4)

extern seg_desc_t GDT[SIZE_GDT];
extern tss_t TSS;


void print_gdt();

void set_each_descriptor(seg_desc_t *seg, uint8_t type, uint8_t dpl, uint32_t base, uint32_t limit);

void init_gdt();

void init_TSS();

#endif