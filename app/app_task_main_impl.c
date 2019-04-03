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
#ifndef APP_TASK_MAIN_IMPL_H
#define APP_TASK_MAIN_IMPL_H

#include "app_task_main_impl.h"

#include "dev_types.h"
#include "debug_helpers.h"
#include "logbuffer.h"
#include "devices.h"
#include "version.h"

#include "app_shared_data.h"
#include "led_gpio_hal.h"
#include "cmsis_os.h"

static const uint32_t MAIN_DETECT_TIMEOUT_TICKS = 5000;

static uint32_t mainloopCount = 0;

typedef enum {
    MAIN_STATE_INIT,
    MAIN_STATE_DETECT,
    MAIN_STATE_RUN,
    MAIN_STATE_ERROR
} MainState;

//MainState getMainState(void);
static MainState mainState = MAIN_STATE_INIT;

//MainState getMainState(void)
//{
//    return mainState;
//}

static uint32_t stateStartTick = 0;
static uint32_t stateTicks(void)
{
    return osKernelSysTick() - stateStartTick;
}

SensorStatus getMiscStatus(const Devices *d)
{
    if (d->sfpiic.present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->vxsiic.present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->eeprom_config.present != DEVICE_NORMAL)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getFpgaStatus(const Dev_fpga *d)
{
    if (d->present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (d->id != FPGA_DEVICE_ID)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getPllStatus(const Dev_pll *d)
{
    if (d->fsm_state == PLL_STATE_ERROR || d->fsm_state == PLL_STATE_FATAL)
        return SENSOR_CRITICAL;
    if (d->present != DEVICE_NORMAL)
        return SENSOR_CRITICAL;
    if (!d->status.sysclk.b.locked)
        return SENSOR_CRITICAL;
    if (!d->status.sysclk.b.stable ||
            !d->status.sysclk.b.pll0_locked ||
            !d->status.sysclk.b.pll1_locked
            )
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}

SensorStatus getSystemStatus(const Devices *dev)
{
    const SensorStatus powermonStatus = getPowermonStatus(&dev->pm);
    const SensorStatus temperatureStatus = dev_thset_thermStatus(&dev->thset);
    const SensorStatus miscStatus = getMiscStatus(dev);
    const SensorStatus fpgaStatus = getFpgaStatus(&dev->fpga);
    const SensorStatus pllStatus = getPllStatus(&dev->pll);
    SensorStatus systemStatus = SENSOR_NORMAL;
    if (powermonStatus > systemStatus)
        systemStatus = powermonStatus;
    if (temperatureStatus > systemStatus)
        systemStatus = temperatureStatus;
    if (miscStatus > systemStatus)
        systemStatus = miscStatus;
    if (fpgaStatus > systemStatus)
        systemStatus = fpgaStatus;
    if (pllStatus > systemStatus)
        systemStatus = pllStatus;
    return systemStatus;
}

void task_main_init(void)
{

}

void task_main_run(void)
{
    mainloopCount++;
    const MainState oldState = mainState;
    const PmState pmState = get_dev_powermon_const()->pmState;
//    led_toggle(LED_YELLOW);

    switch (mainState) {
    case MAIN_STATE_INIT:
        if (pmState == PM_STATE_RUN) {
            struct_Devices_init(getDevices());
            mainState = MAIN_STATE_DETECT;
        }
        break;
    case MAIN_STATE_DETECT:
        if (pmState != PM_STATE_RUN) {
            mainState = MAIN_STATE_INIT;
        }
        if (getDeviceStatus(getDevicesConst()) == DEVICE_NORMAL)
            mainState = MAIN_STATE_RUN;
        if (stateTicks() > MAIN_DETECT_TIMEOUT_TICKS) {
            log_printf(LOG_ERR, "DETECT timeout");
            mainState = MAIN_STATE_ERROR;
        }
        break;
    case MAIN_STATE_RUN:
        if (pmState != PM_STATE_RUN) {
            mainState = MAIN_STATE_INIT;
        }
        break;
    case MAIN_STATE_ERROR:
        if (stateTicks() > 2000) {
            mainState = MAIN_STATE_INIT;
        }
        break;
    }

    if (mainState == MAIN_STATE_DETECT || mainState == MAIN_STATE_RUN) {
        devDetect(getDevices());
    }
    if (mainState == MAIN_STATE_RUN) {
        devRun(getDevices());
    }
    enable_pll_run = (pmState == PM_STATE_RUN);
    if (oldState != mainState) {
        stateStartTick = osKernelSysTick();
    }

    const SensorStatus systemStatus = getSystemStatus(getDevicesConst());
    led_set_state(LED_RED, systemStatus >= SENSOR_CRITICAL);
    led_set_state(LED_YELLOW, systemStatus >= SENSOR_WARNING);
    led_set_state(LED_GREEN, systemStatus == SENSOR_NORMAL);
}

#endif // APP_TASK_MAIN_IMPL_H
