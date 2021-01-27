/* GPLv2 (c) Airbus */
#include <intr.h>
#include <debug.h>
#include <info.h>
#include <pagination.h>
#include <task.h>

extern info_t *info;
extern void idt_trampoline();
static int_desc_t IDT[IDT_NR_DESC];
task_t *current_task;
extern tss_t TSS;
extern void __attribute__((regparm(1))) switch_stack(uint32_t* interrupted_esp, uint32_t* next_esp);

void intr_init()
{
   idt_reg_t idtr;
   offset_t isr;
   size_t i;

   isr = (offset_t)idt_trampoline;

   /* re-use default grub GDT code descriptor */
   for (i = 0; i < IDT_NR_DESC; i++, isr += IDT_ISR_ALGN)
      int_desc(&IDT[i], gdt_krn_seg_sel(1), isr);

   idtr.desc = IDT;
   idtr.limit = sizeof(IDT) - 1;
   set_idtr(idtr);
}

void __regparm__(1) intr32_hdlr(int_ctx_t *ctx)
{
   
   uint32_t* interrupted_esp =(uint32_t*)get_esp();
   debug("\nEnter int32 handler\n");
   uint32_t eip = ctx->eip.raw;
   current_task = current_task->next_task;
   
   //Identify interrupted task
   if(eip>=KERNEL_BEGIN && eip< USER1_BEGIN) {
      debug("Kernel task interrupted \n");
   }
   else if(eip >=USER1_BEGIN && eip <USER2_BEGIN){
      debug("User1 task interrupted \n");
   }
   else if(eip >=USER2_BEGIN && eip <=USER2_END){
      debug("User2 task interrupted \n");
   }

   
   debug("interrupted_esp: %x \n", interrupted_esp);
   debug("next_esp: %x \n", current_task->krn_stack_esp);

   TSS.s0.esp = (uint32_t)current_task->krn_stack_ebp;
   set_cr3(current_task->pgd);
  

   //change stack
   switch_stack(interrupted_esp, current_task->krn_stack_esp);
}

void intr80_hdlr(int_ctx_t *ctx){
   debug("incrementing counter: %d",ctx->gpr.esi);
}

void print_intr(int_ctx_t *ctx){
   debug("\nIDT event\n"
         " . int    #%d\n"
         " . error  0x%x\n"
         " . cs:eip 0x%x:0x%x\n"
         " . ss:esp 0x%x:0x%x\n"
         " . eflags 0x%x\n"
         "\n- GPR\n"
         "eax     : 0x%x\n"
         "ecx     : 0x%x\n"
         "edx     : 0x%x\n"
         "ebx     : 0x%x\n"
         "esp     : 0x%x\n"
         "ebp     : 0x%x\n"
         "esi     : 0x%x\n"
         "edi     : 0x%x\n"
         ,ctx->nr.raw, ctx->err.raw
         ,ctx->cs.raw, ctx->eip.raw
         ,ctx->ss.raw, ctx->esp.raw
         ,ctx->eflags.raw
         ,ctx->gpr.eax.raw
         ,ctx->gpr.ecx.raw
         ,ctx->gpr.edx.raw
         ,ctx->gpr.ebx.raw
         ,ctx->gpr.esp.raw
         ,ctx->gpr.ebp.raw
         ,ctx->gpr.esi.raw
         ,ctx->gpr.edi.raw);
}

void __regparm__(1) intr_hdlr(int_ctx_t *ctx)
{

   uint8_t vector = ctx->nr.blow;

   if (vector == 32)
   {
      //jump to int 32 handler to switch task
      intr32_hdlr(ctx);
   }
   else if (vector == 80)
   {
      //jump to int 80 handler 
      intr80_hdlr(ctx);
   }
   else if (vector < NR_EXCP)
   {
      print_intr(ctx);
      excp_hdlr(ctx);
   }
   else
   {
      debug("ignore IRQ %d\n", vector);
   }
}
