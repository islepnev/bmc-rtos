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

#include "dev_common_types.h"

#include "display.h"

const char *sensor_status_ansi_str(SensorStatus state)
{
    switch(state) {
    case SENSOR_UNKNOWN:  return STR_RESULT_UNKNOWN;
    case SENSOR_NORMAL:   return STR_RESULT_NORMAL;
    case SENSOR_WARNING:  return STR_RESULT_WARNING;
    case SENSOR_CRITICAL: return STR_RESULT_CRIT;
    default: return STR_RESULT_FAIL;
    }
}

const char *sensor_status_text(SensorStatus state)
{
    switch(state) {
    case SENSOR_UNKNOWN:  return "UNKNOWN";
    case SENSOR_NORMAL:   return "NORMAL";
    case SENSOR_WARNING:  return "WARNING";
    case SENSOR_CRITICAL: return "CRITICAL";
    default: return "FAIL";
    }
}

char *deviceStatusResultStr(DeviceStatus status)
{
    switch (status) {
    case DEVICE_UNKNOWN:
        return STR_RESULT_UNKNOWN;
    case DEVICE_NORMAL:
        return STR_RESULT_NORMAL;
    case DEVICE_FAIL:
        return STR_RESULT_FAIL;
    default:
        return "???";
    }
}
