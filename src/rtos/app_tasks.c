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

#include "app_tasks.h"

#include "app_task_cli.h"
#include "app_task_display.h"
#include "app_task_heartbeat.h"
#include "app_task_main.h"
#include "app_task_pll.h"
#include "bsp.h"
#if !defined(BOARD_ADC64VEV1)
#include "app_task_powermon.h"
#endif
#include "bsp.h"
#include "debug_helpers.h"

#if !defined(BOARD_ADC64VEV1) && !defined(BOARD_TDC64VHLEV1) && !defined(BOARD_TDC72) && !defined(BOARD_TDC72VHLV2) && !defined(BOARD_TQDC16VSV1)
#include "ad9516/app_task_auxpll.h"
#endif
#include "fpga/app_task_fpga.h"
#if defined(BOARD_TTVXS)
#include "vxsiicm/app_task_vxsiicm.h"
#else
#if !defined(BOARD_ADC64VEV1) && !defined(BOARD_TDC64VHLEV1) && !defined(BOARD_TDC72VHLV2) && !defined(BOARD_TQDC16VSV1)
#include "vxsiics/app_task_vxsiics.h"
#endif
#endif
#if defined(BOARD_TTVXS) || defined(BOARD_CRU16) || defined(BOARD_TQDC) || defined(BOARD_TDC64VLE)
#include "app_task_tcpip.h"
#endif
#include "app_task_adc.h"

#include "dev_common_types.h"
#include "devicelist.h"

void debug_heap_usage(const char *str)
{
    static size_t prev = 0;
    size_t heapSize = xPortGetFreeHeapSize();
    debug_printf("  [heap: %+6d %6d] created task %s\n", heapSize - prev, heapSize, str);
    prev = heapSize;
}

static DeviceBase topdevice = {0};

void create_tasks(void)
{
    BusInterface bus = {0};
    create_device(0, &topdevice, 0, DEV_CLASS_VIRTUAL, bus, "Device Root");

    // create_task_heartbeat(); // no need, see main task
    create_task_display();
    debug_heap_usage("display");

    create_task_cli();
    debug_heap_usage("cli");

#if !defined(BOARD_ADC64VEV1)
    create_task_powermon(&topdevice);
    debug_heap_usage("powermon");
#endif
    create_task_main();
    debug_heap_usage("main");

#if ENABLE_AD9516
    create_task_auxpll(&topdevice);
    debug_heap_usage("auxpll");
#endif
    create_task_pll(&topdevice);
    debug_heap_usage("pll");
    create_task_fpga(&topdevice);
    debug_heap_usage("fpga");
#ifdef ENABLE_VXSIICM
    create_task_vxsiicm(&topdevice);
    debug_heap_usage("vxsiicm");
#else
#ifdef ENABLE_VXSIICS
    create_task_vxsiics(&topdevice);
    debug_heap_usage("vxsiics");
#endif
#endif
#if defined(BOARD_TTVXS) || defined(BOARD_CRU16) || defined(BOARD_TQDC) || defined(BOARD_TDC64VLE)
    create_task_tcpip();
    debug_heap_usage("tcpip");
#endif
#if !defined(BOARD_ADC64VEV1) && !defined(BOARD_TDC64VHLEV1) && !defined(BOARD_TDC72VHLV2) && !defined(BOARD_TQDC16VSV1)
    create_task_adc();
    debug_heap_usage("adc");
#endif
}
