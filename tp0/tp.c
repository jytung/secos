/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

void tp() {
   debug("kernel mem [0x%x - 0x%x]\n", &__kernel_start__, &__kernel_end__);
   debug("MBI flags 0x%x\n", info->mbi->flags);
   
   
   //uint32_t *ptr_start= (uint32_t*)info->mbi->mmap_addr ;
   uint32_t length= info->mbi->mmap_length;
   
   multiboot_memory_map_t * entry= (multiboot_memory_map_t *)info->mbi->mmap_addr;
   uint32_t count= length/sizeof(multiboot_memory_map_t);
   for(; count!=0;entry++) {
      uint32_t * addr = (uint32_t *)(uint32_t)(entry->addr);
      uint32_t prev_content = *addr;
      debug("addr: %x\t ",   (entry->addr));
      debug("- %x \t",  (entry->len) + (entry->addr));
      debug("(%x) \t",  (entry->type) );
      debug("prev content: 0x%x\t", prev_content);
      *addr = 0x42;
      debug("new content 0x%x\n", *addr);
      
      count--;
   }

/*
   ptr_start[142]=0x55; 
   ptr_start[144]=0x5FF; 
   for(uint32_t i=0; i<length;i++){
      debug("%d :%x \n", i,ptr_start[i] );
   }
   debug("%d :%x \n", 144,ptr_start[144] );
   */
}