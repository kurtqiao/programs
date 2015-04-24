;  Called from main program in CMAIN.C
;  Assemble with Tasm /ml ASMREGS.ASM

.MODEL  small
;Do not place .386p upper .model, would compile code prefix db 66h, cause program fail
.386p

PUBLIC _addval
PUBLIC _ASMREGS

        .CODE
;declare procedure with _ prefix, do not use far
_ASMREGS  PROC
  push bp
  mov bp, sp
  mov bp, [bp+4]				;get STRUCT address from stack
  mov eax, DWORD PTR [bp]			;get STRUCT value
  mov ebx, DWORD PTR [bp+2]
  add ebx, 10
  add eax, ebx
  mov DWORD PTR [bp], eax			;return STRUCT value
  mov DWORD PTR [bp+2], ebx
  pop bp
	ret
_ASMREGS  ENDP


_addval	proc
	push bp
	mov bp, sp
	mov ax, [bp+4]
	mov bx, [bp+6]
	add ax, bx				;return value in ax
	pop bp
	ret
_addval endp

	END
