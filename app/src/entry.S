#ifndef ENTRY_S
#define ENTRY_S

.section      .vectors
	j _entry
	j IRQHandler

.section      .text.entry
.globl _entry

_entry:
//lui	a5,0x2
//addi	a4,zero,1
//sb	a4,1(a5) # 2001
/*	xor tp, tp, tp
	xor ra, ra, ra
	xor t1, t1, t1
	xor t2, t2, t2
	xor s0, s0, s0
	xor s1, s1, s1
	xor a0, a0, a0
	xor a1, a1, a1
	xor a2, a2, a2
	xor a3, a3, a3
	xor a4, a4, a4
	xor a5, a5, a5
	xor a6, a6, a6
	xor a7, a7, a7
	xor s2, s2, s2
	xor s3, s3, s3
	xor a0, a0, a0
	xor s4, s4, s4
	xor s5, s5, s5
	xor s6, s6, s6
	xor s7, s7, s7
	xor s8, s8, s8
	xor s9, s9, s9
	xor s10, s10, s10
	xor s11, s11, s11
	xor t3, t3, t3
	xor t4, t4, t4
	xor t5, t5, t5
	xor t6, t6, t6*/
.option push
	// Ensure the instruction is not optimized, since gp is not yet set.
.option norelax
	// __global_pointer$ is a magic symbol, known by the linker.
	// Unless instructed not to do so, the linker optimizes
	// accesses +/- 2KB around this to gp-relative.
	la gp, __global_pointer$
.option pop

	// The linker script usually defines the stack at the end of RAM.
	la sp, __stack

	// Proceed with the standard standard _begin() routine.

	j _begin


#endif
