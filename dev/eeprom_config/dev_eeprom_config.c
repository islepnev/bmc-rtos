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

#include <stdio.h>

#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "at24c/dev_at24c.h"
#include "dev_common_types.h"
#include "bsp.h"
#include "i2c.h"
#include "bus/i2c_driver.h"
#include "bus/bus_types.h"
#include "bus/impl/i2c_driver_util.h" // FIXME: use index, not handle
#include "system_status_common.h"

DeviceStatus dev_eeprom_config_detect(Dev_eeprom_config *d)
{
    if (at24c_detect(&d->dev.bus))
        d->dev.device_status = DEVICE_NORMAL;
    else
        d->dev.device_status = DEVICE_FAIL;
//    uint8_t data = 0;
//    if (at24c_read(eeprom_Config_busAddress, 0, &data)) {
//        d->present = DEVICE_NORMAL;
//    }
    return d->dev.device_status;
}

DeviceStatus dev_eeprom_config_read(Dev_eeprom_config *d)
{
    uint8_t data = 0;
    if (! at24c_read(&d->dev.bus, 0, &data)) {
        d->dev.device_status = DEVICE_FAIL;
    }
    return d->dev.device_status;
}

void dev_eeprom_config_print(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_EEPROM_CONFIG);
    if (!d || !d->priv)
        return;
    const Dev_eeprom_config_priv *priv = (const Dev_eeprom_config_priv *)device_priv_const(d);

    printf("EEPROM config:  %s", sensor_status_ansi_str(get_eepromConfig_status()));
    printf("%s\n", ANSI_CLEAR_EOL);
}
