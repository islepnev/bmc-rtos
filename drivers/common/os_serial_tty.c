/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "os_serial_tty.h"

#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include "bsp.h"
#include "bsp_tty.h"
#include "cmsis_os.h"
#include "error_handler.h"

typedef struct uart_errors_t {
    uint32_t overrun;
    uint32_t framing;
    uint32_t parity;
    uint32_t noise;
} uart_errors_t;

#define UART_COUNT 6
uart_errors_t uart_errors[UART_COUNT] = {};

osMessageQDef(message_q_ttyrx, 1, uint32_t);
osMessageQDef(message_q_ttytx_1, 1, uint32_t); // tty
osMessageQDef(message_q_ttytx_2, 1, uint32_t); // debug
osMessageQId message_q_ttyrx_id = 0;
osMessageQId message_q_ttytx_1_id = 0;
osMessageQId message_q_ttytx_2_id = 0;

QueueHandle_t get_queue_by_usart(const USART_TypeDef *usart)
{
#ifdef TTY_USART
    if (usart == TTY_USART)
        return message_q_ttytx_1_id;
#endif
#ifdef DEBUG_USART
    if (usart == DEBUG_USART)
        return message_q_ttytx_2_id;
#endif
    return 0;
}

int uart_index(const USART_TypeDef *usart)
{
    if (usart == USART1)
        return 0;
    if (usart == USART2)
        return 1;
    if (usart == USART3)
        return 2;
#ifdef USART4
    if (usart == USART4)
        return 3;
#endif
#ifdef USART5
    if (usart == USART5)
        return 4;
#endif
#ifdef USART6
    if (usart == USART6)
        return 5;
#endif
    return 0;
}

static void USART_TXE_Callback_FromISR(USART_TypeDef *usart)
{
    QueueHandle_t q = get_queue_by_usart(usart);
    if (q) {
        osEvent event = osMessageGet(q, 0);
        if (osOK != event.status && osEventMessage != event.status) {
            // should not happen, ignore
        }
        if (osOK != event.status)
            return;
    }
    LL_USART_DisableIT_TXE(usart);
}

static void USART_RXNE_Callback_FromISR(USART_TypeDef *usart)
{
    char ch;
    ch = LL_USART_ReceiveData8(usart);
    int data = ch;
    if (message_q_ttyrx_id)
        osMessagePut(message_q_ttyrx_id, data, 0);
}

void serial_console_interrupt_handler(USART_TypeDef *usart)
{
    int index = uart_index(usart);
    if (LL_USART_IsActiveFlag_TXE(usart) && LL_USART_IsEnabledIT_TXE(usart)) {
        USART_TXE_Callback_FromISR(usart);
    } else
    if (LL_USART_IsActiveFlag_RXNE(usart) && LL_USART_IsEnabledIT_RXNE(usart)) {
        USART_RXNE_Callback_FromISR(usart);
    } else
    if (LL_USART_IsActiveFlag_ORE(usart)) {
        LL_USART_ClearFlag_ORE(usart);
        uart_errors[index].overrun++;
    } else
    if (LL_USART_IsActiveFlag_FE(usart)) {
        LL_USART_ClearFlag_FE(usart);
        uart_errors[index].framing++;
    } else
    if (LL_USART_IsActiveFlag_PE(usart)) {
        LL_USART_ClearFlag_PE(usart);
        uart_errors[index].parity++;
    } else
    if (LL_USART_IsActiveFlag_NE(usart)) {
        LL_USART_ClearFlag_NE(usart);
        uart_errors[index].noise++;
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

int enqueue_char(USART_TypeDef *usart, int ch)
{
    QueueHandle_t q = get_queue_by_usart(usart);

    if (!q)
        return EIO;
    if (osOK == osMessagePut(q, ch, osWaitForever)) {
        LL_USART_EnableIT_TXE(usart);
        LL_USART_TransmitData8(usart, ch);
    }
    return 0;
}

int __io_putchar(int ch)
{
    int ret = 0;
#ifdef DEBUG_USART
    enqueue_char(DEBUG_USART, ch);
#endif
#ifdef TTY_USART
    ret = enqueue_char(TTY_USART, ch);
#endif
    return ret;
}

void initialize_tty_driver(void)
{
    message_q_ttyrx_id = osMessageCreate(osMessageQ(message_q_ttyrx), NULL);
    message_q_ttytx_1_id = osMessageCreate(osMessageQ(message_q_ttytx_1), NULL);
    message_q_ttytx_2_id = osMessageCreate(osMessageQ(message_q_ttytx_2), NULL);
    if (!message_q_ttyrx_id || !message_q_ttytx_1_id || !message_q_ttytx_2_id)
        Error_Handler();
}
