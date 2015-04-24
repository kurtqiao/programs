CcallASM
========

C main call assembly procedure test program, a simple struct to test parameter passing between C and ASM.

2012.11.15
Configuration:
Build environment:
Borland C++ 4.5, TASM 4.1 or upper

1. run b.bat will set build environment, and run build process
   please modify to your own directory
2. This simple test just define a simple registers struct, then ASMREGS procedure is in asm file
   C program will call ASMREGS, simplely modify eax, ebx and then return value back to C program.
   and addval procedure simplely to add 2 provided value and return to C to print out.