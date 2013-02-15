#as:
#objdump:   -dr
#name:    arith_test1
.*: +file format .*


Disassembly of section .text:

00000000 <_start>:
   0:	0220c005 	xor r3, r4, r5
   4:	00080002 	and r0, r1, r2
   8:	00080002 	and r0, r1, r2
   c:	00080042 	and r0, r1, r2 >> #32
  10:	00080082 	and r0, r1, r2 |> #32
  14:	000800c2 	and r0, r1, r2 <> #33
  18:	08081e42 	add r0, r1, r2 >> #15
  1c:	08081e02 	add r0, r1, r2 << #15
  20:	08081e82 	add r0, r1, r2 |> #15
  24:	08081ec2 	add r0, r1, r2 <> #15
  28:	09081e42 	add.a r0, r1, r2 >> #15
  2c:	09081e02 	add.a r0, r1, r2 << #15
  30:	09081e82 	add.a r0, r1, r2 |> #15
  34:	09081ec2 	add.a r0, r1, r2 <> #15
  38:	08080662 	add r0, r1, r2 >> r3
  3c:	08080622 	add r0, r1, r2 << r3
  40:	080806a2 	add r0, r1, r2 |> r3
  44:	080806e2 	add r0, r1, r2 <> r3
  48:	09080662 	add.a r0, r1, r2 >> r3
  4c:	09080622 	add.a r0, r1, r2 << r3
  50:	090806a2 	add.a r0, r1, r2 |> r3
  54:	090806e2 	add.a r0, r1, r2 <> r3
