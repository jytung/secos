#ifndef __GDT_H__
#define __GDT_H__

#include <segmem.h>
#include <debug.h>
#include <info.h>

#define SIZE_GDT 6
extern seg_desc_t GDT[SIZE_GDT];
extern tss_t TSS;

void print_gdt();

void set_each_descriptor(seg_desc_t *seg, uint8_t type, uint8_t dpl, uint32_t base, uint32_t limit);

void init_gdt();

void init_TSS();

#endif