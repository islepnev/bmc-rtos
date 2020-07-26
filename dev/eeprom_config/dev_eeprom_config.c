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

#include "dev_eeprom_config.h"

#include "app_shared_data.h"
#include "at24c/dev_at24c.h"
#include "dev_common_types.h"
#include "bsp.h"
#include "i2c.h"
#include "bus/i2c_driver.h"
#include "bus/bus_types.h"
#include "bus/impl/i2c_driver_util.h" // FIXME: use index, not handle

Dev_eeprom_config *dev_eeprom_config_init(BusInterface *bus)
{
    Dev_eeprom_config *d = get_dev_eeprom_config();
    d->bus = *bus;
    return d;
}

DeviceStatus dev_eeprom_config_detect(Dev_eeprom_config *d)
{
    if (at24c_detect(&d->bus))
        d->present = DEVICE_NORMAL;
    else
        d->present = DEVICE_FAIL;
//    uint8_t data = 0;
//    if (at24c_read(eeprom_Config_busAddress, 0, &data)) {
//        d->present = DEVICE_NORMAL;
//    }
    return d->present;
}

DeviceStatus dev_eeprom_config_read(Dev_eeprom_config *d)
{
    uint8_t data = 0;
    if (! at24c_read(&d->bus, 0, &data)) {
        d->present = DEVICE_FAIL;
    }
    return d->present;
}

