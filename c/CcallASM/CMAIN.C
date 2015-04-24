/* 
 ASMREGS,addval procedure is in ASMREGS.ASM
 Compile with Borland C++ 4.5: bcc cmain.c ASMREGS.obj
 Copyright KurtQiao@2012.11.15
 Twitter@KurtQiao 

*/

#include <stdio.h>

typedef struct REGISTERS{
	int eax;
	int ebx;
	int ecx;
	int edx;
	int es;
	int di;
}REGS;


extern int ASMREGS( REGS *regs);
extern int addval(int, int);

void main()
{
	
	  REGS regs;
	  
	  regs.eax = 10;
	  regs.ebx = 10;
	  
	  printf("Program to test C call assembly!\nBefore call ASMREGS init ax = %d, bx= %d\n", regs.eax, regs.ebx);
	  
	  ASMREGS(&regs);
	  
    printf( "After ASMREGS: bx+10, ax=ax+bx => ax= %d, bx= %d \n",  regs.eax, regs.ebx);
    
    printf("addval result = %d \n",  addval(3, 5) );
}

