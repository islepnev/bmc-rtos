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

#include "app_task_tcpip.h"

#include "cmsis_os.h"
#include "app_tasks.h"
#include "app_task_tcpip_impl.h"
#include "debug_helpers.h"

osThreadId tcpipThreadId = NULL;
enum { tcpipThreadStackSize = 1024+configMINIMAL_STACK_SIZE * 2 };
static const uint32_t tcpipTaskLoopDelay = 10;

static void start_thread_tcpip( void const *arg)
{
    (void) arg;

    debug_printf("Started thread %s\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
    task_tcpip_init();

    while (1)
    {
        osThreadTerminate(NULL);
    }
}

osThreadDef(tcpip, start_thread_tcpip,    osPriorityLow, 1, tcpipThreadStackSize);

void create_task_tcpip(void)
{
    tcpipThreadId = osThreadCreate(osThread (tcpip), NULL);
    if (tcpipThreadId == NULL) {
        debug_print("Failed to create tcpip thread\n");
    }
}
