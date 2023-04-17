# Archivo: division.s

.global division
.type division, @function

division:
    pushq %rbp           
    movq  %rsp, %rbp    
    
    # Function arguments
    movl 8(%rbp), %eax   
    movl 12(%rbp), %ebx  
    
    xorl %edx, %edx      
    movl $0, %ecx        
    
divide_loop:
    cmpl %eax, %ebx      
    jg   divide_end      
    
    subl %ebx, %eax      
    incl %ecx            
    jmp  divide_loop     
    
divide_end:
    # Function return
    movl %eax, 16(%rbp)  
    movl %ecx, %eax      
    
    popq %rbp            
    ret                  

