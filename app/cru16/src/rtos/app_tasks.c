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

#include "app_tasks.h"
#include "app_task_heartbeat.h"
#include "app_task_powermon.h"
#include "app_task_main.h"
#include "app_task_display.h"
#include "app_task_cli.h"
#include "app_task_pll.h"
#include "app_task_auxpll.h"
#include "app_task_fpga.h"
#include "app_task_vxsiic.h"
#include "app_task_tcpip.h"

void create_tasks(void)
{
    create_task_heartbeat();
    create_task_display();
    create_task_cli();
    create_task_powermon();
    create_task_main();
    create_task_pll();
    create_task_auxpll();
    create_task_fpga();
    create_task_vxsiic();
    create_task_tcpip();
}
