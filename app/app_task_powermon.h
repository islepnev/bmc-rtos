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
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef APP_TASK_POWERMON_H
#define APP_TASK_POWERMON_H

#include "dev_powermon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PM_STATE_INIT,
    PM_STATE_STANDBY, // bus power switches off
    PM_STATE_RAMP_5V, // voltage ramp up
    PM_STATE_RAMP,    // voltage ramp up
    PM_STATE_RUN,
    PM_STATE_PWRFAIL,
    PM_STATE_ERROR
} PmState;

PmState getPmState(void);
Dev_powermon getPmData(void);
uint32_t getPmLoopCount(void);
SensorStatus getPowermonStatus(const Dev_powermon *pm);
void create_task_powermon(void);

#ifdef __cplusplus
}
#endif

#endif // APP_TASK_POWERMON_H
