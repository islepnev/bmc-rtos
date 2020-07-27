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

#include "device_status_log.h"

#include <assert.h>

#include "log/log.h"

LogPriority device_status_log_priority(DeviceStatus status)
{
    switch(status) {
    case DEVICE_UNKNOWN:
        return  LOG_ERR;
    case DEVICE_NORMAL:
        return LOG_INFO;
    case DEVICE_FAIL:
        return LOG_ERR;
    default:
        assert(0);
        return LOG_CRIT;
    }
}

void dev_log_status_change(BusInterface *bus, DeviceStatus status)
{
    LogPriority prio = device_status_log_priority(status);
    const char *text = "";
    switch(status) {
    case DEVICE_UNKNOWN:
        text = "not found";
        break;
    case DEVICE_NORMAL:
        text = "Ok";
        break;
    case DEVICE_FAIL:
        text = "failed";
        break;
    default:
        assert(0);
    }
    const char *bus_type_str = "";
    switch (bus->type) {
    case BUS_IIC: bus_type_str = "IIC"; break;
    case BUS_SPI: bus_type_str = "SPI"; break;
    default: assert(0);
    }
    log_printf(prio, "Device %s on %s %d.%02X",
               text,
               bus_type_str,
               bus->bus_number,
               bus->address);
}

LogPriority sensor_status_log_priority(SensorStatus status)
{
    switch(status) {
    case SENSOR_UNKNOWN:
        return  LOG_INFO;
    case SENSOR_NORMAL:
        return LOG_INFO;
    case SENSOR_WARNING:
        return LOG_WARNING;
    case SENSOR_CRITICAL:
        return LOG_CRIT;
    default:
        assert(0);
        return LOG_CRIT;
    }
}

void sensor_log_status_change(BusInterface *bus, SensorStatus status)
{
    LogPriority prio = sensor_status_log_priority(status);
    const char *text = "";
    switch(status) {
    case SENSOR_UNKNOWN:
        text = "not found";
        break;
    case SENSOR_NORMAL:
        text = "Ok";
        break;
    case SENSOR_WARNING:
        text = "warning";
        break;
    case SENSOR_CRITICAL:
        text = "critical";
        break;
    default:
        assert(0);
    }
    const char *bus_type_str = "";
    switch (bus->type) {
    case BUS_IIC: bus_type_str = "IIC"; break;
    case BUS_SPI: bus_type_str = "SPI"; break;
    default: assert(0);
    }
    log_printf(prio, "Sensor %s on %s %d.%02X",
               text,
               bus_type_str,
               bus->bus_number,
               bus->address);
}
