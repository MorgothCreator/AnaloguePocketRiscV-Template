#include <stdint.h>
#include "sysreg.h"
#include "riscv_hal.h"

#ifdef __cplusplus
extern "C" {
#endif
int IntDefaultHandler(unsigned int reason);

//*****************************************************************************
//
// Macro for weak symbol aliasing
//
//*****************************************************************************
#define WEAK_ALIAS(x) __attribute__ ((weak, alias(#x)))
volatile int dummy_interrupt_return;
/*------------------------------------------------------------------------------
 *
 */
int IRQHandler_1(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_2(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_3(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_4(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_5(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_6(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_7(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_8(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_9(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_10(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_11(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_12(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_13(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_14(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_15(unsigned int) WEAK_ALIAS(IntDefaultHandler);
int IRQHandler_16(unsigned int) WEAK_ALIAS(IntDefaultHandler);


int (* const g_pfnVectors[])(unsigned int) =
{
	IRQHandler_1,
	IRQHandler_2,
	IRQHandler_3,
	IRQHandler_4,
	IRQHandler_5,
	IRQHandler_6,
	IRQHandler_7,
	IRQHandler_8,
	IRQHandler_9,
	IRQHandler_10,
	IRQHandler_11,
	IRQHandler_12,
	IRQHandler_13,
	IRQHandler_14,
	IRQHandler_15,
	IRQHandler_16,
};


ISR(IRQHandler)
{
#ifdef USE_INTERRUPTS
	ISR_PROLOGUE();
	int interrupt_nr;
	__asm__ __volatile__ (
		"la %0, (0x14)\n"
		"lw %0, 0(%0)"
	: "=r" (interrupt_nr)
	:);

	int (*int_to_call)(unsigned int) = g_pfnVectors[interrupt_nr];
	int_to_call(interrupt_nr);

	ISR_EPILOGUE();
#endif
}

/*------------------------------------------------------------------------------
 *
 */
/*------------------------------------------------------------------------------
 * RISC-V interrupt handler for external interrupts.
 */
/*__attribute__ ((section(".vectors"), used))
void vector_table()
{
	asm volatile ("j _entry");
	asm volatile ("j IRQHandler");
}*/

//*****************************************************************************
//
//! This is the code that gets called when the processor receives an unexpected
//! interrupt. This simply enters an infinite loop, preserving the system state
//! for examination by a debugger.
//
//*****************************************************************************
int IntDefaultHandler(unsigned int reason)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}
/*------------------------------------------------------------------------------
 * Disable all interrupts.
 */
void __disable_irq(void)
{
    //clear_csr(mstatus, MSTATUS_MPIE);
    //clear_csr(mstatus, MSTATUS_MIE);
}

/*------------------------------------------------------------------------------
 * Enabler all interrupts.
 */
void __enable_irq(void)
{
    //set_csr(mstatus, MSTATUS_MIE);
}

#ifdef __cplusplus
}
#endif
