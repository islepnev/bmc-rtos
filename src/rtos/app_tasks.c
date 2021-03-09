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
#include "app_task_powermon.h"
#include "bsp.h"

#if !defined(BOARD_TDC72)
#include "ad9516/app_task_auxpll.h"
#endif
#include "fpga/app_task_fpga.h"
#if defined(BOARD_TTVXS)
#include "vxsiicm/app_task_vxsiicm.h"
#else
#include "vxsiics/app_task_vxsiics.h"
#endif
#if defined(BOARD_TTVXS) || defined(BOARD_CRU16) || defined(BOARD_TQDC)
#include "app_task_tcpip.h"
#endif
#include "app_task_adc.h"

#include "dev_common_types.h"
#include "devicelist.h"

static DeviceBase topdevice = {0};

void create_tasks(void)
{
    BusInterface bus = {0};
    create_device(0, &topdevice, 0, DEV_CLASS_VIRTUAL, bus, "Device Root");

    // create_task_heartbeat(); // no need, see main task
    create_task_display();
    create_task_cli();
    create_task_powermon(&topdevice);
    create_task_main();
#if ENABLE_AD9516
    create_task_auxpll(&topdevice);
#endif
    create_task_pll(&topdevice);
    create_task_fpga(&topdevice);
#ifdef ENABLE_VXSIICM
    create_task_vxsiicm(&topdevice);
#else
#ifdef ENABLE_VXSIICS
    create_task_vxsiics(&topdevice);
#endif
#endif
#if defined(BOARD_TTVXS) || defined(BOARD_CRU16) || defined(BOARD_TQDC)
    create_task_tcpip();
#endif
    create_task_adc();
}
