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

#include "app_task_heartbeat.h"

#include <stdint.h>

#include "app_tasks.h"
#include "bsp.h"
#include "cmsis_os.h"
#include "debug_helpers.h"
#include "led_gpio_hal.h"

enum { heartbeatThreadStackSize = threadStackSize };

static const int mainQUEUE_SEND_FREQUENCY_MS = 200;

osMessageQDef(message_q, 1, uint32_t); // Declare a message queue, size 1
osMessageQId (message_q_id);           // Declare an ID for the message queue

static void task_heartbeat_rtos(void)
{
    led_toggle(LED_HEARTBEAT);
}

static void prvQueueSendTask(void const *arg)
{
    (void) arg;
    const uint32_t ulValueToSend = 100UL;

    // debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));

    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again. */
//        vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );
        osDelay(mainQUEUE_SEND_FREQUENCY_MS);

        /* Send to the queue - causing the queue receive task to unblock and
        toggle the LED.  0 is used as the block time so the sending operation
        will not block - it shouldn't need to block as the queue should always
        be empty at this point in the code. */
        uint32_t data = ulValueToSend;
        osMessagePut(message_q_id, data, 0);
    }
}

static void prvQueueReceiveTask(void const *arg)
{
    (void) arg;
    uint32_t ulReceivedValue;
    const uint32_t ulExpectedValue = 100UL;

    // debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));

    for( ;; )
    {
        osEvent event = osMessageGet(message_q_id, osWaitForever);
        ulReceivedValue = event.value.v;

        /*  To get here something must have been received from the queue, but
        is it the expected value?  If it is, toggle the LED. */
        if( ulReceivedValue == ulExpectedValue )
        {
            task_heartbeat_rtos();
            ulReceivedValue = 0U;
        }
    }
}

osThreadDef(heartbeat_deq, prvQueueReceiveTask, osPriorityLow,  1, heartbeatThreadStackSize);
osThreadDef(heartbeat_enq, prvQueueSendTask,    osPriorityIdle, 1, heartbeatThreadStackSize);

void create_task_heartbeat(void)
{
    message_q_id = osMessageCreate(osMessageQ(message_q), NULL);

    if( message_q_id != NULL )
    {
        osThreadId rxThreadId = osThreadCreate(osThread (heartbeat_deq), NULL);
        osThreadId txThreadId = osThreadCreate(osThread (heartbeat_enq), NULL);
        if (rxThreadId == NULL) {
            debug_print("Failed to create heartbeat_deq thread\n");
        }
        if (txThreadId == NULL) {
            debug_print("Failed to create heartbeat_enq thread\n");
        }
    }
}
