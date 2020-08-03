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

#ifndef SFPIIC_IIC_DRIVER_H
#define SFPIIC_IIC_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#include "bus/bus_types.h"

#ifdef  __cplusplus
extern "C" {
#endif

enum { PCA9548_BASE_I2C_ADDRESS = 0x74, SFP_MAIN_I2C_ADDRESS = 0x50};

bool sfpiic_pca9548_detect(BusInterface *bus);
bool sfpiic_pca9548_set_channel(BusInterface *bus, uint8_t channel);
// bool sfpiic_pca9548_read(BusInterface *bus, uint8_t *pData, uint16_t Size);
// bool sfpiic_pca9548_write(BusInterface *bus, uint8_t *pData, uint16_t Size);
bool sfpiic_mem_read(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
bool sfpiic_mem_write(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
bool sfpiic_get_ch_i2c_status(BusInterface *bus);

#ifdef  __cplusplus
}
#endif

#endif // SFPIIC_IIC_DRIVER_H
