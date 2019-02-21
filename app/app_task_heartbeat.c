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

#include "FreeRTOS.h"
#include "queue.h"

#include "app_shared_data.h"

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 200 / portTICK_PERIOD_MS )

static QueueHandle_t xQueue = NULL;

static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

void task_heartbeat_rtos(void)
{
    dev_leds_toggle(&dev.leds, LED_GREEN);
}


static void prvQueueSendTask( void *pvParameters )
{
TickType_t xNextWakeTime;
const unsigned long ulValueToSend = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again. */
        vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

        /* Send to the queue - causing the queue receive task to unblock and
        toggle the LED.  0 is used as the block time so the sending operation
        will not block - it shouldn't need to block as the queue should always
        be empty at this point in the code. */
        xQueueSend( xQueue, &ulValueToSend, 0U );
    }
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
unsigned long ulReceivedValue;
const unsigned long ulExpectedValue = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    for( ;; )
    {
        /* Wait until something arrives in the queue - this task will block
        indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
        FreeRTOSConfig.h. */
        xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

        /*  To get here something must have been received from the queue, but
        is it the expected value?  If it is, toggle the LED. */
        if( ulReceivedValue == ulExpectedValue )
        {
            task_heartbeat_rtos();
            ulReceivedValue = 0U;
        }
    }
}

void create_task_heartbeat(int rx_priority, int tx_priority)
{
    /* The number of items the queue can hold.  This is 1 as the receive task
    will remove items as they are added, meaning the send task should always find
    the queue empty. */
    const int mainQUEUE_LENGTH = 1;
    xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );
    if( xQueue != NULL )
    {
        xTaskCreate( prvQueueReceiveTask,				// The function that implements the task.
                     "Rx", 								// The text name assigned to the task - for debug only as it is not used by the kernel.
                     configMINIMAL_STACK_SIZE, 			// The size of the stack to allocate to the task.
                     NULL, 								// The parameter passed to the task - not used in this case.
                     rx_priority, 	// The priority assigned to the task.
                     NULL );								// The task handle is not required, so NULL is passed.

        xTaskCreate( prvQueueSendTask,
                     "TX",
                     configMINIMAL_STACK_SIZE,
                     NULL,
                     tx_priority,
                     NULL );
    }
}
