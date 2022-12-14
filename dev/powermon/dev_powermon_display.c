/*
**    Copyright 2020 Ilja Slepnev
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

#include "dev_powermon_display.h"

#include <stdio.h>

#include "ansi_escape_codes.h"
#include "bsp_powermon.h"
#include "dev_pm_sensors.h"
#include "dev_powermon_types.h"
#include "display.h"

const char *monStateStr(MonState state)
{
    switch(state) {
    case MON_STATE_INIT:    return "INIT";
    case MON_STATE_DETECT:    return ANSI_YELLOW "RAMP"    ANSI_CLEAR;
    case MON_STATE_READ:     return ANSI_GREEN  "RUN"     ANSI_CLEAR;
    case MON_STATE_ERROR:   return ANSI_RED    "ERROR"   ANSI_CLEAR;
    default: return "?";
    }
}

const char *pmStateStr(PmState state)
{
    switch(state) {
    case PM_STATE_INIT:    return "INIT";
    case PM_STATE_WAITINPUT: return "WAIT-INPUT";
    case PM_STATE_STANDBY: return "STANDBY";
    case PM_STATE_RAMP:    return ANSI_YELLOW "RAMP"    ANSI_CLEAR;
    case PM_STATE_RUN:     return ANSI_GREEN  "RUN"     ANSI_CLEAR;
    case PM_STATE_OFF:     return ANSI_GRAY   "OFF"     ANSI_CLEAR;
    case PM_STATE_PWRFAIL: return ANSI_RED    "POWER SUPPLY FAILURE" ANSI_CLEAR;
    case PM_STATE_FAILWAIT: return ANSI_RED    "POWER SUPPLY FAILURE" ANSI_CLEAR;
    case PM_STATE_ERROR:   return ANSI_RED    "ERROR"   ANSI_CLEAR;
    case PM_STATE_OVERHEAT: return ANSI_RED    "OVERHEAT" ANSI_CLEAR;
    default: return "?";
    }
}

void pm_sensor_print_header(void)
{
    printf("%10s %7s %6s %6s %5s\n", "       ", "  V  ", "  A  ", " A max ", "  W  ");
}

void pm_sensor_print_values(const struct pm_sensor *d, bool isOn)
{
    SensorStatus sensorStatus = pm_sensor_status(d);
    const pm_sensor_priv *sensor = &d->priv;
    int offvoltage = !isOn && (sensor->busVoltage > 0.1);
    const char *color = "";
    switch (sensorStatus) {
    case SENSOR_UNKNOWN:  color = sensor->isOptional ? ANSI_GRAY : ANSI_YELLOW; break;
    case SENSOR_NORMAL:   color = ANSI_GREEN;  break;
    case SENSOR_WARNING:  color = ANSI_YELLOW; break;
    case SENSOR_CRITICAL: color = sensor->isOptional ? ANSI_YELLOW : ANSI_RED;    break;
    }
    printf("%s % 7.3f%s", isOn ? color : offvoltage ? ANSI_YELLOW : "", sensor->busVoltage, ANSI_CLEAR);
    if (sensor->hasShunt) {
        int backfeed = (sensor->current < -0.010);
        printf("%s % 6.3f %s% 6.3f % 5.1f", backfeed ? ANSI_YELLOW : "", sensor->current, backfeed ? ANSI_CLEAR : "",
               sensor->currentMax, sensor->power);
    } else {
        printf("         ");
    }
    //        double sensorStateDuration = pm_sensor_get_sensorStatus_Duration(d) / getTickFreqHz();
}

void pm_sensor_print(const pm_sensor *d, int isOn)
{
    printf("%10s", d->priv.label);
    if (d->dev.device_status == DEVICE_NORMAL) {
        pm_sensor_print_values(d, isOn);
        printf(" %s", isOn ? sensor_status_ansi_str(d->dev.sensor) : STR_RESULT_OFF);
    } else {
        printf(" %s", STR_RESULT_UNKNOWN);
    }
}

static void monPrintValues(const Dev_powermon_priv *p)
{
    pm_sensor_print_header();
    {
        const pm_sensors_arr *sensors = &p->sensors;
        for (int i=0; i<sensors->count; i++) {
            pm_sensor_print(&sensors->arr[i], monIsOn(p->sw, (SensorIndex)i));
            printf("\n");
        }
    }
}

void print_sensors_box(void)
{
    const Dev_powermon_priv *priv = get_powermon_priv_const();
    if (!priv)
        return;
    SensorStatus sensorStatus = pm_sensors_getStatus(priv);
    printf("VME power: %4.1f W, %4.1f W max, FPGA: %4.1f W %s\n",
           pm_get_power_w(priv),
           pm_get_power_max_w(priv),
           pm_get_fpga_power_w(priv),
           sensor_status_ansi_str(sensorStatus));
    monPrintValues(priv);
}

static void print_pm_switches(const pm_switches sw)
{
    printf("Switch: ");
    for (int i=0; i<POWER_SWITCH_COUNT; i++) {
        printf("%s %s   ", psw_label((PowerSwitchIndex)i), sw[i] ? STR_ON : STR_OFF);
    }
    printf("\n");
}

static void pm_pgood_print(const pm_pgoods pgood)
{
    //    printf("Live insert: %s\n", pm.vmePresent ? STR_RESULT_ON : STR_RESULT_OFF);
    printf("Power good: ");
    for (int i=0; i<POWER_GOOD_COUNT; i++) {
        printf("%s %s   ", pgood_label((PowerGoodIndex)i), pgood[i] ? STR_ON : STR_OFF);
    }
    printf("\n");
}

void print_powermon_box(void)
{
    const Dev_powermon_priv *priv = get_powermon_priv_const();
    if (!priv)
        return;
    const PmState pmState = get_powermon_state();
    //    print_clearbox(DISPLAY_POWERMON_Y, DISPLAY_POWERMON_H);
    printf("Powermon: %-20s   Sensors: %s\n", pmStateStr(pmState), monStateStr(priv->monState));
//    if (pmState == PM_STATE_INIT) {
//        print_clearbox(DISPLAY_POWERMON_Y+1, DISPLAY_POWERMON_H-1);
//    } else {
    print_pm_switches(priv->sw_state);
    pm_pgood_print(priv->pgood);
//    }
}
