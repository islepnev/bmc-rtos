//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "os_serial_tty.h"

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "cmsis_os.h"
#include "stm32f7xx_ll_usart.h"

osMessageQDef(message_q_ttyrx, 1, uint32_t);
osMessageQDef(message_q_ttytx, 1, uint32_t);
osMessageQId (message_q_ttyrx_id);
osMessageQId (message_q_ttytx_id);

USART_TypeDef * const tty_usart = USART3;

static void USART_TXE_Callback_FromISR(USART_TypeDef *usart)
{
    osEvent event = osMessageGet(message_q_ttytx_id, 0);
    if (osOK != event.status && osEventMessage != event.status) {
        // should not happen, ignore
    }
    if (osOK == event.status) {
        LL_USART_DisableIT_TXE(usart);
    }
}

static void USART_RXNE_Callback_FromISR(USART_TypeDef *usart)
{
    char ch;
    ch = LL_USART_ReceiveData8(usart);
    int data = ch;
    if (osOK == osMessagePut(message_q_ttyrx_id, data, 0)) {}
}

void serial_console_interrupt_handler(USART_TypeDef *usart)
{
    if (LL_USART_IsActiveFlag_TXE(usart) && LL_USART_IsEnabledIT_TXE(usart)) {
        USART_TXE_Callback_FromISR(usart);
    }
    if (LL_USART_IsActiveFlag_RXNE(usart) && LL_USART_IsEnabledIT_RXNE(usart)) {
        USART_RXNE_Callback_FromISR(usart);
    }
    if (LL_USART_IsActiveFlag_ORE(usart)) {
        LL_USART_ClearFlag_ORE(usart);
    }
}

int __io_getchar (void)
{
    if (!message_q_ttyrx_id)
        return EIO;
    osEvent event = osMessageGet(message_q_ttyrx_id, osWaitForever);
    if (osEventMessage != event.status) {
        for(;;) {}
    }

    return (int)event.value.v;
}

int __io_putchar(int ch)
{
    if (!message_q_ttytx_id)
        return EIO;
    if (osOK == osMessagePut(message_q_ttytx_id, ch, osWaitForever)) {
        LL_USART_EnableIT_TXE(tty_usart);
        LL_USART_TransmitData8(tty_usart, ch);
    }
    return 0;
}

void initialize_serial_console_hardware(void)
{
    message_q_ttyrx_id = osMessageCreate(osMessageQ(message_q_ttyrx), NULL);
    message_q_ttytx_id = osMessageCreate(osMessageQ(message_q_ttytx), NULL);
    LL_USART_EnableIT_RXNE(tty_usart);
}
