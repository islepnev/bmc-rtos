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
#include <stdlib.h>

#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "cmsis_os.h"
#include "display.h"
#include "RunTimeStatsTimer.h"
#include "stm32_hal.h"

enum {MAX_TASK_COUNT = 30};
typedef struct task_item {
    uint8_t index;
    uint8_t prio;
    eTaskState state;
    uint16_t stack;
    uint32_t runtime;
    char name[configMAX_TASK_NAME_LEN];
} task_item;

unsigned int task_count;
uint32_t total_time;
static task_item task_list[MAX_TASK_COUNT];

static int cmp_task_items(const void *p1, const void *p2)
{
    task_item *i1 = (task_item *)p1;
    task_item *i2 = (task_item *)p2;
    return (int)i1->stack - (int)i2->stack;
}

void sort_tasks()
{
    qsort(&task_list[0], task_count, sizeof(task_list[0]), &cmp_task_items);
}

const char *task_state_str(eTaskState eCurrentState) {
    switch( eCurrentState )
    {
    case eRunning: return "X";
    case eReady: return "R";
    case eBlocked: return "B";
    case eSuspended: return "S";
    case eDeleted: return "D";
    case eInvalid: // Fall through
    default: return "?";
    }
}

void print_tasks()
{
    const double freq = getRunTimeCounterFrequency();
    for (int i=0; i<task_count; i++) {
        printf("%-15s\t%s\t%u\t%u\t%u\t%4.1f\t%9.3f\n",
               task_list[i].name,
               task_state_str(task_list[i].state),
               task_list[i].prio,
               task_list[i].stack,
               task_list[i].index,
               task_list[i].runtime * 100.0 / total_time,
               task_list[i].runtime / freq
               );
    }
}

void print_task_list(void)
{
    unsigned int uxArraySize = uxTaskGetNumberOfTasks();

    TaskStatus_t *pxTaskStatusArray = pvPortMalloc(
        uxTaskGetNumberOfTasks() * sizeof(TaskStatus_t));

    if (pxTaskStatusArray != NULL) {
        /* Generate the (binary) data. */
        uint32_t ulTotalTime;
        uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalTime);
        for (int i = 0; i < uxArraySize; i++) {
            task_list[i].index = pxTaskStatusArray[i].xTaskNumber;
            task_list[i].state = pxTaskStatusArray[i].eCurrentState;
            task_list[i].prio  = pxTaskStatusArray[i].uxCurrentPriority;
            task_list[i].stack = pxTaskStatusArray[i].usStackHighWaterMark;
            task_list[i].runtime = pxTaskStatusArray[i].ulRunTimeCounter;
            strncpy(task_list[i].name, pxTaskStatusArray[i].pcTaskName, configMAX_TASK_NAME_LEN-1);
        }
        total_time = ulTotalTime;
        task_count = uxArraySize;
        vPortFree( pxTaskStatusArray );
        sort_tasks();
        print_tasks();
    }
}

static void print_osThreadList(void)
{
    printf("Name          State  Priority  Stack   Num       CPU        Time\n");
    static const char *div = "-----------------------------------------------------------------\n";
    printf("%s", div);
    print_task_list();
    printf("%s", div);
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

void display_tasks_page(int y)
{
    int taskCount = uxTaskGetNumberOfTasks();
    int sysinfo_lines = 2;
    int threadlist_y = y + sysinfo_lines + 1;
    int threadlist_lines = 4 + taskCount;
//    int runstats_y = threadlist_y + threadlist_lines + 1;
//    int runstats_lines = 2 + taskCount;
    print_goto(y, 1);
    print_sysinfo_brief();
    print_clear_eol();

    print_clearbox(threadlist_y, threadlist_lines);
    print_goto(threadlist_y, 1);
    print_osThreadList();
    print_clear_eol();
    int cur_y = threadlist_y + threadlist_lines;

//    print_clearbox(runstats_y, runstats_lines);
//    print_goto(runstats_y, 1);
//    print_RunTimeStats();
//    print_clear_eol();
//    int cur_y = runstats_y+runstats_lines;
    print_clearbox(cur_y, screen_height - cur_y - 1);
}
