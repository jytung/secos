.text

.globl switch_stack
.type  switch_stack,"function"

switch_stack:
    # eax = second argument = new kernel stack 

    # pushl    %ebp  

    # load new stack       
	# movl     %esp,(%ecx)
    # new esp = new kernel stack pointer  
    mov     %eax,%esp  

    # popl     %ebp 
    # resume from intr       
	popa
    add     $8, %esp
    iret              