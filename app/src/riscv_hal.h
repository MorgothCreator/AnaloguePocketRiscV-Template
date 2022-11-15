/*
 * riscv_hal.h
 *
 *  Created on: Dec 18, 2018
 *      Author: Work
 */

#ifndef RISCV_HAL_H_
#define RISCV_HAL_H_

#ifndef _VECTOR
#define _VECTOR(N) __vector_ ## N
#endif

#define ISR_PROLOGUE() \
	asm volatile 							\
	(										\
		"addi sp, sp, -33*4" "\n\t"			\
		"sw	x1,2*4(sp)" "\n\t"				\
		"sw	x2,3*4(sp)" "\n\t"				\
		"sw	x3,4*4(sp)" "\n\t"				\
		"sw	x4,5*4(sp)" "\n\t"				\
		"sw	x5,6*4(sp)" "\n\t"				\
		"sw	x6,7*4(sp)" "\n\t"				\
		"sw	x7,8*4(sp)" "\n\t"				\
		"sw	x8,9*4(sp)" "\n\t"				\
		"sw	x9,10*4(sp)" "\n\t"				\
		"sw	x10,11*4(sp)" "\n\t"			\
		"sw	x11,12*4(sp)" "\n\t"			\
		"sw	x12,13*4(sp)" "\n\t"			\
		"sw	x13,14*4(sp)" "\n\t"			\
		"sw	x14,15*4(sp)" "\n\t"			\
		"sw	x15,16*4(sp)" "\n\t"			\
		"sw	x16,17*4(sp)" "\n\t"			\
		"sw	x17,18*4(sp)" "\n\t"			\
		"sw	x18,19*4(sp)" "\n\t"			\
		"sw	x19,20*4(sp)" "\n\t"			\
		"sw	x20,21*4(sp)" "\n\t"			\
		"sw	x21,22*4(sp)" "\n\t"			\
		"sw	x22,23*4(sp)" "\n\t"			\
		"sw	x23,24*4(sp)" "\n\t"			\
		"sw	x24,25*4(sp)" "\n\t"			\
		"sw	x25,26*4(sp)" "\n\t"			\
		"sw	x26,27*4(sp)" "\n\t"			\
		"sw	x27,28*4(sp)" "\n\t"			\
		"sw	x28,29*4(sp)" "\n\t"			\
		"sw x29,30*4(sp)" "\n\t"			\
		"sw	x30,31*4(sp)" "\n\t"			\
		"sw	x31,32*4(sp)" "\n\t"			\
		"lw	t1,20(zero)" "\n\t"	\
		"sw	t1,1*4(sp)" "\n\t"	/* Store Interrupt number, REG_MCAUSE*/	\
		"lw	t1,8(zero)" "\n\t"	/* Load return address*/\
		"sw	t1,0*4(sp)" "\n\t"	/* Store return address*/	\
	)

#define ISR_EPILOGUE()						\
	asm volatile 							\
	(										\
		"lw	t1,0*4(sp)" "\n\t"	/* Load return address*/\
		"sw	t1,8(zero)" "\n\t"				\
		"lw	x1,2*4(sp)" "\n\t"				\
		"lw	x2,3*4(sp)" "\n\t"				\
		"lw	x3,4*4(sp)" "\n\t"				\
		"lw	x4,5*4(sp)" "\n\t"				\
		"lw	x5,6*4(sp)" "\n\t"				\
		"lw	x6,7*4(sp)" "\n\t"				\
		"lw	x7,8*4(sp)" "\n\t"				\
		"lw	x8,9*4(sp)" "\n\t"				\
		"lw	x9,10*4(sp)" "\n\t"				\
		"lw	x10,11*4(sp)" "\n\t"			\
		"lw	x11,12*4(sp)" "\n\t"			\
		"lw	x12,13*4(sp)" "\n\t"			\
		"lw	x13,14*4(sp)" "\n\t"			\
		"lw	x14,15*4(sp)" "\n\t"			\
		"lw	x15,16*4(sp)" "\n\t"			\
		"lw	x16,17*4(sp)" "\n\t"			\
		"lw	x17,18*4(sp)" "\n\t"			\
		"lw	x18,19*4(sp)" "\n\t"			\
		"lw	x19,20*4(sp)" "\n\t"			\
		"lw	x20,21*4(sp)" "\n\t"			\
		"lw	x21,22*4(sp)" "\n\t"			\
		"lw	x22,23*4(sp)" "\n\t"			\
		"lw	x23,24*4(sp)" "\n\t"			\
		"lw	x24,25*4(sp)" "\n\t"			\
		"lw	x25,26*4(sp)" "\n\t"			\
		"lw	x26,27*4(sp)" "\n\t"			\
		"lw	x27,28*4(sp)" "\n\t"			\
		"lw	x28,29*4(sp)" "\n\t"			\
		"lw x29,30*4(sp)" "\n\t"			\
		"lw	x30,31*4(sp)" "\n\t"			\
		"lw	x31,32*4(sp)" "\n\t"			\
		"addi sp, sp, 33*4" "\n\t"			\
		"mret" "\n\t"			/* Execute return*/		\
	)



#ifdef __cplusplus
#  define ISR(vector)            \
	extern "C" void vector (void) __attribute__ ((naked)); \
    void vector (void)
#else
#  define ISR(vector, ...)            \
    void vector (void) __attribute__ ((naked)); \
    void vector (void)
#endif

#  define ISR_BLOCK
#  define ISR_NOBLOCK    __attribute__((interrupt))
#  define ISR_NAKED      __attribute__((naked))
#  define ISR_ALIASOF(v) __attribute__((alias(__STRINGIFY(v))))


/*
 *Return value from External IRQ handler. This will be used to disable the External
 *interrupt.
 */
#define EXT_IRQ_KEEP_ENABLED                0U
#define EXT_IRQ_DISABLE                     1U

/*------------------------------------------------------------------------------
 * Interrupt enable/disable.
 */
void __disable_irq(void);
void __enable_irq(void);


#endif /* RISCV_HAL_H_ */
