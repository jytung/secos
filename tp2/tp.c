/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>
#include <segmem.h>


extern info_t *info;

void interruption_divide_by_zero(){
    while(1){
        int i=100;
        int zero=0;
        i=i/zero;
    }
}

void test_sti()
{
   while (1) asm volatile ("sti");
}

void BP_handler(){
    
    asm volatile("pusha");
    debug("Enter BP handler\n");
    uint32_t eip;
    //eip est juste apres ebp 
    asm volatile ("mov 4(%%ebp), %0":"=r"(eip));
    debug("EIP = %p\n", eip);
    debug("End BP handler\n");
    //general protection fault --> il ya  un push ebp(genere par le code c) avant le pusha
    // il faut bien quitter le bp handler
    //asm volatile("popa;iret");
    asm volatile("popa;leave;iret");
    
}

void BP_trigger(){
    asm volatile("int3");
    debug("BP triggered\n");
}

void test_BP(){
    idt_reg_t idtr;
    get_idtr(idtr);
    debug("@IDT:%p\n", &idtr.addr);

    //descriptor 3 is reserved for breakpoint interruption
    int_desc_t *BP_desc= &idtr.desc[3];
    BP_desc->offset_1= (uint16_t)((uint32_t)BP_handler);
    BP_desc->offset_2= (uint16_t)((uint32_t)BP_handler>>16);
    
    BP_trigger();
}


void tp()
{
    //test_sti();
    test_BP();
    //BP_handler();
}
