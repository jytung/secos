#ifndef __PAGINATION_H__
#define __PAGINATION_H__

#include <debug.h>
#include <info.h>
#include <pagemem.h>
#include <cr.h>

#define SHM_ADDR 0XC10000

#define CPT_USER1_ADDR 0x401000
#define CPT_USER2_ADDR 0x801000

void pagination();

#endif