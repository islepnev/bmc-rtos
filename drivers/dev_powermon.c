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

#include "dev_powermon.h"
#include "ansi_escape_codes.h"
#include "display.h"
#include "dev_pm_sensors.h"
#include "cmsis_os.h"

int monIsOn(const pm_switches sw, SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5: return sw.switch_1v5;
    case SENSOR_5V: return sw.switch_5v;
    case SENSOR_VME_5V: return 1;
    case SENSOR_3V3: return sw.switch_3v3;
    case SENSOR_VME_3V3: return 1;
    case SENSOR_FPGA_CORE_1V0: return sw.switch_1v0;
    case SENSOR_FPGA_MGT_1V0: return sw.switch_1v0;
    case SENSOR_FPGA_MGT_1V2: return sw.switch_1v0;
    case SENSOR_FPGA_1V8: return sw.switch_3v3;
    case SENSOR_TDC_A: return sw.switch_3v3;
    case SENSOR_TDC_B: return sw.switch_3v3;
    case SENSOR_TDC_C: return sw.switch_3v3;
    case SENSOR_CLOCK_2V5: return sw.switch_3v3;
    }
    return 0;
}

void struct_powermon_sensors_init(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_init(&d->sensors[i], i);
    }
}

void struct_powermon_init(Dev_powermon *d)
{
    d->monState = MON_STATE_INIT;
    d->stateStartTick = 0;
    d->monErrors = 0;
    d->monCycle = 0;
    struct_powermon_sensors_init(d);
//    d->present = DEVICE_UNKNOWN;
    d->vmePresent = 0;
    d->fpga_core_pgood = 0;
    d->ltm_pgood = 0;
    d->sw.switch_5v = 1;
    d->sw.switch_3v3 = 1;
    d->sw.switch_1v5 = 1;
    d->sw.switch_1v0 = 1;
}

static int readPowerGoodFpga(void)
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(FPGA_CORE_PGOOD_GPIO_Port, FPGA_CORE_PGOOD_Pin));
}

static int readPowerGood1v5(void)
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(LTM_PGOOD_GPIO_Port, LTM_PGOOD_Pin));
}

static int readLiveInsertPin(void)
{
    GPIO_PinState state;
    state = HAL_GPIO_ReadPin(VME_DET_B_GPIO_Port, VME_DET_B_Pin);
    return (GPIO_PIN_RESET == state);
}

int pm_read_liveInsert(Dev_powermon *pm)
{
    pm->vmePresent = readLiveInsertPin();
    return pm->vmePresent;
}

void pm_read_pgood(Dev_powermon *pm)
{
    pm->fpga_core_pgood = readPowerGoodFpga();
    pm->ltm_pgood = readPowerGood1v5();
}

PgoodState get_all_pgood(const Dev_powermon *pm)
{
    return (pm->fpga_core_pgood && pm->ltm_pgood) ? PGOOD_OK : PGOOD_FAIL;
}

void update_power_switches(Dev_powermon *pm, SwitchOnOff state)
{
//    pm_read_pgood(pm);
    pm->sw.switch_5v  = state; // && pm_sensor_isValid(&pm->sensors[SENSOR_VME_5V]); // VME 5V and 3.3V
    pm->sw.switch_1v5 = state; // monBusValid(0x42); // 5V
    pm->sw.switch_1v0 = state; // && pm_sensor_isValid(&pm->sensors[SENSOR_1V5]); //pm->ltm_pgood; // 1.5V
    pm->sw.switch_3v3 = state; // && pm_sensor_isValid(&pm->sensors[SENSOR_VME_3V3]); // pm->fpga_core_pgood && pm->sw.switch_1v0 && pm->sw.switch_1v5;
    if (!pm->sw.switch_5v) {
        pm->sw.switch_3v3 = 0;
        pm->sw.switch_1v5 = 0;
        pm->sw.switch_1v0 = 0;
    }
    if (!pm->sw.switch_1v5) {
        pm->sw.switch_1v0 = 0;
        pm->sw.switch_3v3 = 0;
    }
    if (!pm->sw.switch_1v0) {
        pm->sw.switch_3v3 = 0;
    }
    HAL_GPIO_WritePin(ON_1_0V_1_2V_GPIO_Port, ON_1_0V_1_2V_Pin, pm->sw.switch_1v0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ON_1_5V_GPIO_Port,      ON_1_5V_Pin,      pm->sw.switch_1v5 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ON_3_3V_GPIO_Port,      ON_3_3V_Pin,      pm->sw.switch_3v3 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ON_5V_GPIO_Port,        ON_5V_Pin,        pm->sw.switch_5v  ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

int pm_sensors_isAllValid(const Dev_powermon *d)
{
    for (int i=0; i < POWERMON_SENSORS; i++)
        if (!pm_sensor_isValid(&d->sensors[i]))
            return 0;
    return 1;
}
SensorStatus pm_sensors_getStatus(const Dev_powermon *d)
{
    SensorStatus maxStatus = SENSOR_NORMAL;
    for (int i=0; i < POWERMON_SENSORS; i++) {
        SensorStatus status = pm_sensor_status(&d->sensors[i]);
        if (status > maxStatus)
            maxStatus = status;
    }
    return maxStatus;
}

void monClearMeasurements(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_clear_measurements(&d->sensors[i]);
    }
}

int monDetect(Dev_powermon *d)
{
    int count = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        if (pm_sensor_detect(&d->sensors[i]))
            count++;
    }
    return count;
}

int monReadValues(Dev_powermon *d)
{
    int err = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        err += pm_sensor_read(&d->sensors[i]);
//        printMonValue(deviceAddr, busVolt * 1.25e-3, shuntVolt * 2.5e-6, monShuntVal[i]);
    }
    return err;
}

void pm_setStateStartTick(Dev_powermon *pm)
{
    pm->stateStartTick = HAL_GetTick();
}

uint32_t getMonStateTicks(const Dev_powermon *pm)
{
    return HAL_GetTick() - pm->stateStartTick;
}

MonState runMon(Dev_powermon *pm)
{
    pm->monCycle++;
    const MonState oldState = pm->monState;
    if (!pm->sw.switch_5v) {
        pm->monState = MON_STATE_INIT;
        return 1;
    }
    switch(pm->monState) {
    case MON_STATE_INIT:
        struct_powermon_sensors_init(pm);
        pm->monState = MON_STATE_DETECT;
        break;
    case MON_STATE_DETECT:
        // all but two devices up
        if (monDetect(pm) > POWERMON_SENSORS - 2)
            pm->monState = MON_STATE_READ;
        break;
    case MON_STATE_READ:
        if (monReadValues(pm) == 0)
            pm->monState = MON_STATE_READ;
        else
            pm->monState = MON_STATE_ERROR;
        break;
    case MON_STATE_ERROR:
        pm->monErrors++;
        pm->monState = MON_STATE_INIT;
        break;
    default:
        break;
    }
    if (oldState != pm->monState) {
        pm_setStateStartTick(pm);
    }
    return pm->monState;
}

int getSensorIsValid_5V(const Dev_powermon *pm)
{
    return pm_sensor_isValid(&pm->sensors[SENSOR_5V]);
}

int getSensorIsValid_3V3(const Dev_powermon *pm)
{
    return pm_sensor_isValid(&pm->sensors[SENSOR_3V3]);
}
