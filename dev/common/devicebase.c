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

#include "devicebase.h"

#include <assert.h>

#include "device_status_log.h"

const char *device_class_str(DeviceClass c)
{
    switch (c) {
    case DEV_CLASS_0: return "none";
    case DEV_CLASS_VIRTUAL: return "Virtual";
    case DEV_CLASS_AD5141: return "AD5141";
    case DEV_CLASS_AD9516: return "AD9516";
    case DEV_CLASS_CLKMUX: return "clkmux";
    case DEV_CLASS_DIGIPOTS: return "DigiPots";
    case DEV_CLASS_EEPROM: return "EEPROM";
    case DEV_CLASS_FPGA: return "FPGA";
    case DEV_CLASS_INA226: return "INA226";
    case DEV_CLASS_AD9545: return "AD9545";
    case DEV_CLASS_AD9548: return "AD9548";
    case DEV_CLASS_PCA9548: return "PCA9548";
    case DEV_CLASS_POWERMON: return "PowerMon";
    case DEV_CLASS_SFP: return "SFP";
    case DEV_CLASS_SFPIIC: return "SFPIIC";
    case DEV_CLASS_THSET: return "Thermometers";
    case DEV_CLASS_ADT7301: return "ADT7301";
    case DEV_CLASS_MAX31725: return "MAX31725";
    case DEV_CLASS_TMP421: return "TMP421";
    case DEV_CLASS_VXSIICM: return "VXSIICM";
    case DEV_CLASS_VXSIICS: return "VXSIICS";
    default:
        assert(0);
        return "?";
    }
}

const char *bus_type_str(BusType t)
{
    switch (t) {
    case BUS_NONE: return "";
    case BUS_IIC: return "IIC";
    case BUS_SPI: return "SPI";
    default: assert(0); return "?";
    }
}

void set_device_status(DeviceBase *d, const DeviceStatus status)
{
    if (status == d->device_status)
        return;
    d->device_status = status;
    // dev_log_status_change(d);
}
