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

#include "display_tasks.h"

#include <stdio.h>
#include <string.h>

#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "cmsis_os.h"
#include "display.h"
#include "stm32f7xx_hal.h"

static void print_osThreadList(void)
{
    static portCHAR PAGE_BODY[1024];
    printf("Name          State  Priority  Stack   Num\n" );
    printf("---------------------------------------------\n");
    /* The list of tasks and their status */
    PAGE_BODY[0] = '\0';
    osThreadList((unsigned char *)(PAGE_BODY));
    printf("%s", PAGE_BODY);
    printf("---------------------------------------------\n");
    printf("B : Blocked, R : Ready, D : Deleted, S : Suspended\n");
}

static void print_RunTimeStats(void)
{
    static char statsBuffer[1000];
    char *buf = statsBuffer;
    strcpy(buf, "Task");
    buf += strlen(buf);
    for(int i = strlen("Task"); i < ( configMAX_TASK_NAME_LEN - 3 ); i++) {
        *buf = ' ';
        buf++;
        *buf = '\0';
    }
    const char *hdr = "  Abs Time      % Time\r\n****************************************" ANSI_CLEAR_EOL "\n";
    strcpy(buf, hdr);
    buf += strlen(hdr);
    vTaskGetRunTimeStats(buf);
    printf("%s", statsBuffer);
}

static void print_sysinfo_brief(void)
{
    printf("FreeRTOS %s, CMSIS %u.%u, CMSIS-OS %u.%u", tskKERNEL_VERSION_NUMBER,
           __CM_CMSIS_VERSION >> 16, __CM_CMSIS_VERSION & 0xFFFF,
           osCMSIS >> 16, osCMSIS & 0xFFFF
           );
    printf("%s\n", ANSI_CLEAR_EOL ANSI_CLEAR);
    printf("CPU %lX rev %lX, HAL %lX, UID %08lX-%08lX-%08lX",
           HAL_GetDEVID(), HAL_GetREVID(),
           HAL_GetHalVersion(),
           HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2()
           );
    printf("%s\n", ANSI_CLEAR_EOL ANSI_CLEAR);
}

void display_tasks(int y)
{
    int taskCount = uxTaskGetNumberOfTasks();
    int sysinfo_lines = 2;
    int threadlist_y = y + sysinfo_lines + 1;
    int threadlist_lines = 4 + taskCount;
    int runstats_y = threadlist_y + threadlist_lines + 1;
    int runstats_lines = 2 + taskCount;
    print_goto(y, 1);
    print_sysinfo_brief();
    print_clear_eol();

    print_clearbox(threadlist_y, threadlist_lines);
    print_goto(threadlist_y, 1);
    print_osThreadList();
    print_clear_eol();

    print_clearbox(runstats_y, runstats_lines);
    print_goto(runstats_y, 1);
    print_RunTimeStats();
    print_clear_eol();
    int cur_y = runstats_y+runstats_lines;
    print_clearbox(cur_y, screen_height - cur_y - 1);
}
