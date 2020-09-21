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

#include "display_sensors.h"

#include "bsp_sensors_config.h"
#include "display.h"
#include "powermon/dev_powermon_display.h"
#include "powermon/dev_powermon_types.h"

// const int DISPLAY_SENSORS_HEIGHT = (POWERMON_SENSORS+2);

void print_sensors(int y)
{
#if defined(ENABLE_POWERMON) && defined (ENABLE_SENSORS)
    const PmState pmState = get_powermon_state();
    if (pmState == PM_STATE_INIT) {
        print_clearbox(y, DISPLAY_SENSORS_HEIGHT);
    } else {
        print_goto(y, 1);
        print_sensors_box();
    }
#endif
}
