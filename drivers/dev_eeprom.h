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

#ifndef DEV_EEPROM_H
#define DEV_EEPROM_H

#include "dev_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Dev_at24c;

DeviceStatus dev_eepromConfig_detect(struct Dev_at24c *d);
DeviceStatus dev_eepromConfig_read(struct Dev_at24c *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_EEPROM_H
