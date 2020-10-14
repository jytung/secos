/* GPLv2 (c) Airbus */
#ifndef __INFO_DATA_H__
#define __INFO_DATA_H__

#include <types.h>
#include <mbi.h>
#include <segmem.h>

typedef struct information
{
   mbi_t *mbi;
   seg_desc_t *segDes;
   gdt_reg_t *gdt;
} __attribute__((packed)) info_t;


#endif
