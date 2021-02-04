#ifndef __PAGINATION_H__
#define __PAGINATION_H__

#include <debug.h>
#include <info.h>
#include <pagemem.h>
#include <cr.h>



/*******    Kernel         ********/
#define KERNEL_BEGIN    0x300000
//#define CPT_KRN_ADDR    0x301000
#define KERNEL_T1_STACK 0x310000
#define KERNEL_T2_STACK 0x320000

//allocate PGD and PTB
#define KRN_PGD         0x390000
#define U1_PGD          0x391000
#define U2_PGD          0x392000

#define KRN_PTB     0x393000
#define U1_PTB      0x394000
#define U2_PTB      0x395000
#define U1_KRN_PTB  0x396000
#define U2_KRN_PTB  0x397000
#define U1_SHM_PTB  0x398000
#define U2_SHM_PTB  0x399000

/*******    User 1         ********/
#define USER1_BEGIN     0x400000
#define CPT_USER1_ADDR  0x401000
#define USER1_STACK     0x4A0000

/*******    User 2         ********/
#define USER2_BEGIN     0x800000
#define CPT_USER2_ADDR  0x801000
#define USER2_STACK     0x8A0000
#define USER2_END       0xBFFFFF
/*******    Shared memory  ********/
#define SHM_ADDR        0XC10000


void pagination();

#endif