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

#include "app_task_init.h"

#include <stdlib.h>

#include "bsp.h"
#include "cmsis_os.h"
#include "commands.h"
#include "log/log.h"
#include "os_serial_tty.h"
#include "clock.h"

void app_task_init(void)
{
    configureTimerForRunTimeStats();
     test_timers();
    initialize_serial_console_hardware();
    log_put(LOG_NOTICE, "Initializing");
    commands_init();
}
