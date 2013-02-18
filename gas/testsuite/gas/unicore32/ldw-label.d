#as:
#objdump:   -dr
#name:    ldw-label
.*: +file format .*


Disassembly of section .text:

00000000 <func>:
   0:	79fbc004 	ldw r15, \[pc\+\], #4
   4:	787b8000 	stw r14, \[r15\+\]
   8:	00000000 	and r0, r0, r0
			8: R_UNICORE32_ABS32	g
