/*
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

#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "core_cm7.h"

/* Simple shift divide for scaling to avoid an overflow occurring too soon. */
#define runtimeSHIFT_13				13
#define runtimeOVERFLOW_BIT_13		( 1UL << ( 32UL - runtimeSHIFT_13 ) )
static const uint32_t prescale_bits = runtimeSHIFT_13;
static const uint32_t overflow_bit = runtimeOVERFLOW_BIT_13;

void configureTimerForRunTimeStats( void )
{
    /* Enable TRCENA. */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* Enable Lock Acccess */
    DWT->LAR = 0xC5ACCE55;

    /* Reset counter. */
    DWT->CYCCNT = 0;

    /* Enable counter. */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t getRunTimeCounterValue( void )
{
    static uint32_t last_value = 0, overflows = 0;
    uint32_t cycles = DWT->CYCCNT;

    /* Has the value overflowed since it was last read. */
    if ( cycles < last_value )
    {
        overflows += overflow_bit;
    }
    last_value = cycles;

    /* There is no prescale on the counter, so simulate in software. */
    cycles = ( cycles >> prescale_bits ) + overflows;

    return cycles;
}

