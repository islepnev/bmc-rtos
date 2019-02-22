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

#include "cpu_cycle.h"
#include "stm32f7xx.h"

void enable_cpu_cycle_counter( void )
{
   // enable DWT access
   CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
   // enable the CPU cycle counter
   DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t cpu_getCycles(void)
{
   return DWT->CYCCNT;
}
