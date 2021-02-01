.text

.globl switch_stack
.type  switch_stack,"function"

switch_stack:
    # eax = second argument = new kernel stack esp  

    # load new stack       
	# movl     %esp,(%ecx)
    # save interrupted esp 
    #mov     %esp, %edx
    # new esp = new kernel stack pointer 
    mov     %eax,%esp  
         

    # resume from intr       
	popa
    add     $8, %esp
    iret              