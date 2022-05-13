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

#include <assert.h>
#include <stdlib.h>

#include "ansi_escape_codes.h"
#include "app_name.h"
#include "bsp.h"
#include "bsp_tty.h"
#include "clock.h"
#include "cmsis_os.h"
#include "commands.h"
#include "debug_helpers.h"
#include "init_periph.h"
#include "log/log.h"
#include "os_serial_tty.h"

void app_task_init(void)
{
    initialize_tty_driver();
    bsp_tty_setup_uart();

    debug_print(ANSI_CLEAR "\n");
    debug_print(APP_DESCR_FULL "\n");
    configureTimerForRunTimeStats();
    test_timers();
    init_logging();
    commands_init();

    init_periph();
}
