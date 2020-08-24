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

#include <assert.h>

#include "app_tasks.h"
#include "cmsis_os.h"
#include "lwipopts.h"
#include "tcpip/app_task_tcpip_impl.h"

osThreadId tcpipThreadId = NULL;
enum { tcpipThreadStackSize = configMINIMAL_STACK_SIZE + 240 };
static const uint32_t tcpipTaskLoopDelay = 10;

static void start_thread_tcpip( void const *arg)
{
    (void) arg;
    task_tcpip_init();

    while (1)
    {
        osThreadTerminate(NULL);
    }
}

osThreadDef(tcpip, start_thread_tcpip, TCPIP_THREAD_PRIO, 1, tcpipThreadStackSize);

void create_task_tcpip(void)
{
    tcpipThreadId = osThreadCreate(osThread (tcpip), NULL);
    assert(tcpipThreadId);
}
