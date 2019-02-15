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
#include "ftoa.h"
#include "dev_pm_sensors.h"

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
    d->fpga_core_pgood = 0;
    d->ltm_pgood = 0;
    d->sw.switch_5v = 1;
    d->sw.switch_3v3 = 1;
    d->sw.switch_1v5 = 1;
    d->sw.switch_1v0 = 1;
}

int readPowerGoodFpga(void)
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(FPGA_CORE_PGOOD_GPIO_Port, FPGA_CORE_PGOOD_Pin));
}

int readPowerGood1v5(void)
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(LTM_PGOOD_GPIO_Port, LTM_PGOOD_Pin));
}

void print_pm_switches(const pm_switches sw)
{
    printf("Switch 5V %s   3.3V %s   1.5V %s   1.0V %s\n",
           sw.switch_5v  ? STR_ON : STR_OFF,
           sw.switch_3v3 ? STR_ON : STR_OFF,
           sw.switch_1v5 ? STR_ON : STR_OFF,
           sw.switch_1v0 ? STR_ON : STR_OFF);
}

void pm_read_pgood(Dev_powermon *pm)
{
    pm->fpga_core_pgood = readPowerGoodFpga();
    pm->ltm_pgood = readPowerGood1v5();
}

void update_power_switches(Dev_powermon *pm, SwitchOnOff state)
{
//    pm_read_pgood(pm);
    pm->sw.switch_5v  = state; // monBusValid(0x43); // && monBusValid(0x45); // VME 5V and 3.3V
    pm->sw.switch_1v5 = state; // monBusValid(0x42); // 5V
    pm->sw.switch_1v0 = state; //pm->ltm_pgood; // && monBusValid(0x40); // 1.5V
    pm->sw.switch_3v3 = state; // pm->fpga_core_pgood && pm->sw.switch_1v0 && pm->sw.switch_1v5; // && monBusValid(0x45);
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
    HAL_GPIO_WritePin(GPIOC, ON_1_0V_1_2V_Pin, pm->sw.switch_1v0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOJ, ON_1_5V_Pin,      pm->sw.switch_1v5 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOJ, ON_3_3V_Pin,      pm->sw.switch_3v3 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOJ, ON_5V_Pin,        pm->sw.switch_5v  ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void pm_pgood_print(const Dev_powermon pm)
{
    printf("Intermediate 1.5V: %s\n", pm.ltm_pgood ? STR_RESULT_NORMAL : pm.sw.switch_1v5 ? STR_RESULT_FAIL : STR_RESULT_OFF);
    printf("FPGA Core 1.0V:    %s\n", pm.fpga_core_pgood ? STR_RESULT_NORMAL : pm.sw.switch_1v0 ? STR_RESULT_FAIL : STR_RESULT_OFF);
}


int getPowerMonState(const Dev_powermon d)
{
    for (int i=0; i < POWERMON_SENSORS; i++)
        if (!pm_sensor_isValid(d.sensors[i]))
            return 0;
    return 1;
}

const char *monStateStr(int monState)
{
    switch(monState) {
    case MON_STATE_INIT: return "INIT";
    case MON_STATE_DETECT: return "DETECT";
    case MON_STATE_READ: return "READ";
    case MON_STATE_ERROR: return "ERROR";
    default: return "?";
    }
}

void monPrintValues(const Dev_powermon d)
{
    printf("Mon state: %s %s", monStateStr(d.monState), d.monErrors ? STR_RESULT_FAIL : STR_RESULT_NORMAL);
    if (d.monErrors)
        printf("     %d errors", d.monErrors);
    printf("\n");
    if (d.monState == MON_STATE_READ) {
        for (int i=0; i<POWERMON_SENSORS; i++) {
//            uint16_t deviceAddr = monAddr[i];
//            printMonValue(deviceAddr, monValuesBus[i], monValuesShunt[i], monShuntVal(deviceAddr));
            pm_sensor_print(d.sensors[i], monIsOn(d.sw, i));
        }
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

void runMon(Dev_powermon *pm)
{
    pm->monCycle++;
    const MonState oldState = pm->monState;
    if (!pm->sw.switch_5v) {
        pm->monState = MON_STATE_INIT;
        return;
    }
    switch(pm->monState) {
    case MON_STATE_INIT:
        struct_powermon_sensors_init(pm);
        pm->monState = MON_STATE_DETECT;
        break;
    case MON_STATE_DETECT:
        if (monDetect(pm) > 0)
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
}
