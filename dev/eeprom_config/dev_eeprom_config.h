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

#ifndef DEV_EEPROM_CONFIG_H
#define DEV_EEPROM_CONFIG_H

#include <stdint.h>

#include "bus/bus_types.h"
#include "dev_common_types.h"

typedef struct Dev_eeprom_config {
    BusInterface bus;
    DeviceStatus present;
} Dev_eeprom_config;

Dev_eeprom_config *dev_eeprom_config_init(BusInterface *bus);
DeviceStatus dev_eeprom_config_detect(struct Dev_eeprom_config *d);
DeviceStatus dev_eeprom_config_read(struct Dev_eeprom_config *d);

#endif // DEV_EEPROM_CONFIG_H
