.global _start
_start:
    xor r3, r4, r5
    and r0, r1, r2
    and r0, r1, r2 << #0
    and r0, r1, r2 >> #32
    and r0, r1, r2 |> #32
    and r0, r1, r2 <> #33
    add r0, r1, r2 >> #15
    add r0, r1, r2 << #15
    add r0, r1, r2 |> #15
    add r0, r1, r2 <> #15
    add.a r0, r1, r2 >> #15
    add.a r0, r1, r2 << #15
    add.a r0, r1, r2 |> #15
    add.a r0, r1, r2 <> #15
    add r0, r1, r2 >> r3
    add r0, r1, r2 << r3
    add r0, r1, r2 |> r3
    add r0, r1, r2 <> r3
    add.a r0, r1, r2 >> r3
    add.a r0, r1, r2 << r3
    add.a r0, r1, r2 |> r3
    add.a r0, r1, r2 <> r3
