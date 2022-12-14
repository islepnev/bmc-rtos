/*
**    Cortex M-7 Exception Handlers
**
**    Copyright 2019 Ilja Slepnev
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "cm7_exception_handlers.h"
#include <stdint.h>

void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
    // These are volatile to try and prevent the compiler/linker optimising them
    // away as the variables never actually get used.  If the debugger won't show the
    // values of the variables, make them global by moving their declaration outside
    // of this function.
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr;  // Link register
    volatile uint32_t pc;  // Program counter
    volatile uint32_t psr; // Program status register

    r0  = pulFaultStackAddress[ 0 ];
    r1  = pulFaultStackAddress[ 1 ];
    r2  = pulFaultStackAddress[ 2 ];
    r3  = pulFaultStackAddress[ 3 ];
    r12 = pulFaultStackAddress[ 4 ];
    lr  = pulFaultStackAddress[ 5 ];
    pc  = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    // suppress unused variable warning
    (void) r0;
    (void) r1;
    (void) r2;
    (void) r3;
    (void) r12;
    (void) lr;
    (void) pc;
    (void) psr;

    // When the following line is hit, the variables contain the register values
    for( ;; );
}

void NMI_Handler(void)
{
}

__attribute__ (( aligned(8) ))
void HardFault_Handler(void)
{
    //    static char msg[80];
    //    debug_print("In Hard Fault Handler\n");
    //    sprintf(msg, "SCB->HFSR = 0x%08lx\n", SCB->HFSR);
    //    debug_print(msg);
    //    if ((SCB->HFSR & (1 << 30)) != 0) {
    //        debug_print("Forced Hard Fault\n");
    //        sprintf(msg, "SCB->CFSR = 0x%08lx\n", SCB->CFSR );
    //        debug_print(msg);
    //        if(SCB->BFAR != 0) {
    //            debug_printf("BFAR = 0x%08lx\n", SCB->BFAR);
    //        }
    //        if(SCB->MMFAR != 0) {
    //            debug_printf("MMFAR = 0x%08lx\n", SCB->MMFAR);
    //        }
    //        if((SCB->CFSR & 0xFFFF0000) != 0) {
    //            debug_print("usage error");
    //        }
    //    }

    __asm volatile
        (
            " tst lr, #4                                                \n"
            " ite eq                                                    \n"
            " mrseq r0, msp                                             \n"
            " mrsne r0, psp                                             \n"
            " ldr r1, [r0, #24]                                         \n"
            " ldr r2, handler2_address_const                            \n"
            " bx r2                                                     \n"
            " handler2_address_const: .word prvGetRegistersFromStack    \n"
            );

    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    while (1)
    {
    }
}

void BusFault_Handler(void)
{
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

void DebugMon_Handler(void)
{
}
