
#include <stdio.h>

int main() {
    int dividendo = 20, divisor = 3, cociente, resto;
    asm volatile (
        "movl %1, %%eax;"     
        "movl %2, %%ebx;"     
        "xorl %%edx, %%edx;"  
        "divl %%ebx;"         
        "movl %%eax, %0;"    
        "movl %%edx, %3;"     
        : "=r" (cociente), "=r" (resto)
        : "r" (divisor), "r" (0), "0" (dividendo)
        : "%eax", "%ebx", "%edx"
    );
    printf("cociente: %d, resto: %d\n", cociente, resto);
    return 0;
}

