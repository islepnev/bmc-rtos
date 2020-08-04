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

#include "dev_pm_sensors_fsm.h"

#include "bsp.h"
#include "bsp_pin_defs.h"
#include "bsp_powermon.h"
#include "bsp_powermon_types.h"
#include "bsp_sensors_config.h"
#include "cmsis_os.h"
#include "dev_pm_sensors.h"
#include "dev_pm_sensors_types.h"
#include "dev_pm_sensors_util.h"
#include "dev_powermon_types.h"
#include "devicelist.h"
#include "gpio.h"
#include "log/log.h"

int old_num_detected = 0;
int stable_detect_count = 0;

static const uint32_t DETECT_TIMEOUT_TICKS = 1000;

uint32_t getMonStateTicks(const Dev_powermon *pm)
{
    return osKernelSysTick() - pm->priv.stateStartTick;
}

MonState runMon(Dev_powermon *pm)
{
    pm->priv.monCycle++;
    const MonState oldState = pm->priv.monState;
    switch(pm->priv.monState) {
    case MON_STATE_INIT:
        monClearMeasurements(pm);
        pm->priv.monState = MON_STATE_DETECT;
        break;
    case MON_STATE_DETECT: {
        const int num_detected = monDetect(pm);
        if (num_detected == 0) {
            pm->priv.monState = MON_STATE_INIT;
            break;
        }
        if (num_detected > 0 && (old_num_detected == num_detected))
            stable_detect_count++;
        else
            stable_detect_count = 0;
        old_num_detected = num_detected;
        if (stable_detect_count >= 2) {
            update_board_version(num_detected);
            log_printf(LOG_INFO, "%d sensors detected", num_detected);
            pm->priv.monState = MON_STATE_READ;
            break;
        }

        if (getMonStateTicks(pm) > DETECT_TIMEOUT_TICKS) {
            log_printf(LOG_ERR, "Sensor detect timeout, %d sensors found", num_detected);
            pm->priv.monState = MON_STATE_READ;
        }
        break;
    }
    case MON_STATE_READ:
        if (monReadValues(pm) == 0)
            pm->priv.monState = MON_STATE_READ;
        else
            pm->priv.monState = MON_STATE_ERROR;
        break;
    case MON_STATE_ERROR:
        log_put(LOG_ERR, "Sensor read error");
        pm->priv.monErrors++;
        pm->priv.monState = MON_STATE_INIT;
        break;
    default:
        break;
    }
    if (oldState != pm->priv.monState) {
        pm_setStateStartTick(pm);
    }
    return pm->priv.monState;
}
