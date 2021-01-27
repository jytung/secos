.text

.globl switch_stack
.type  switch_stack,"function"

switch_stack:
    push    %ebp          
	mov     %esp,(%eax)  
    mov     %edx,%esp  
    pop     %ebp        
	ret              