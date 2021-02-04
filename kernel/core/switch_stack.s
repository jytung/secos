.text

.globl switch_stack
.type  switch_stack,"function"

switch_stack:
    push    %ebp
    # save interrupted esp 
    mov     %esp, (%eax)
    # new esp = new kernel stack pointer 
    mov     %edx,%esp  
    pop     %ebp     
    ret