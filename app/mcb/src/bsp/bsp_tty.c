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

#include "bsp.h"
#include "bsp_tty.h"
#include "devices_types.h"
#include "mcb.h"
#include "stm32f7xx_ll_usart.h"

void bsp_tty_setup_uart(void)
{
    int pcb_ver = get_mcb_pcb_ver();
    if (pcb_ver == PCB_VER_A_MCB_1_0) {
        LL_USART_SetRXPinLevel(TTY_USART, LL_USART_RXPIN_LEVEL_INVERTED);
        LL_USART_SetTXPinLevel(TTY_USART, LL_USART_TXPIN_LEVEL_INVERTED);
    }
    LL_USART_ConfigAsyncMode(TTY_USART);
    LL_USART_Enable(TTY_USART);
}
