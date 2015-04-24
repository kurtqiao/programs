set path=C:\BC45\BIN;C:\MASM611\BIN;D:\TASM\BIN

tasm /ml ASMREGS.asm

bcc cmain.c asmregs.obj