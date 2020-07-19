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
#ifndef DEV_POWERMON_TYPES_H
#define DEV_POWERMON_TYPES_H

typedef enum SwitchOnOff {
    SWITCH_OFF = 0,
    SWITCH_ON = 1,
} SwitchOnOff;

typedef struct pm_switches {
    SwitchOnOff switch_5v;
    SwitchOnOff switch_3v3;
    SwitchOnOff switch_1v5;
    SwitchOnOff switch_1v0;
    SwitchOnOff switch_tdc_a;
    SwitchOnOff switch_tdc_b;
    SwitchOnOff switch_tdc_c;
#ifdef TDC64
    SwitchOnOff switch_tdc_d;
#endif
} pm_switches;

typedef enum MonState {
    MON_STATE_INIT = 0,
    MON_STATE_DETECT = 1,
    MON_STATE_READ = 2,
    MON_STATE_ERROR = 3
} MonState;

typedef enum {
    PM_STATE_INIT,
    PM_STATE_STANDBY,
    PM_STATE_RAMP,
    PM_STATE_RUN,
    PM_STATE_OFF,
    PM_STATE_PWRFAIL,
    PM_STATE_OVERHEAT
} PmState;

#endif // DEV_POWERMON_TYPES_H
