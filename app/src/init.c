/*******************************************************************************
 * (c) Copyright 2016-2018 Microsemi SoC Products Group.  All rights reserved.
 *
 * @file init.c
 * @author Microsemi SoC Products Group
 * @brief Mi-V soft processor memory section initializations and start-up code.
 *
 * SVN $Revision: 9661 $
 * SVN $Date: 2018-01-15 16:13:33 +0530 (Mon, 15 Jan 2018) $
 */

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif


extern uint32_t     __data_load_addr__;
extern uint32_t     __data_start__;
extern uint32_t     __data_end__;

extern uint32_t     __bss_start;
extern uint32_t     __bss_end;


static void copy_section(uint32_t * p_load, uint32_t * p_vma, uint32_t * p_vma_end)
{
    while(p_vma < p_vma_end)
    {
        *p_vma = *p_load;
        ++p_load;
        ++p_vma;
    }
}

static void zero_section(uint32_t * start, uint32_t * end)
{
    uint32_t * p_zero = start;

    while(p_zero < end)
    {
        *p_zero = 0;
        ++p_zero;
    }
}

void _begin(void)
{
    copy_section(&__data_load_addr__, &__data_start__, &__data_end__);
    zero_section(&__bss_start, &__bss_end);
    asm volatile ("j _start");
}

#ifdef __cplusplus
}
#endif
