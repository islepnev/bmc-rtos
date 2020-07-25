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

#ifdef  __cplusplus
extern "C" {
#endif

enum { PCA9548_BASE_I2C_ADDRESS = 0x74, SFP_MAIN_I2C_ADDRESS = 0x50};

void sfpiic_master_reset(void);
void sfpiic_switch_reset(void);
bool sfpiic_device_detect(uint16_t addr);
bool sfpiic_switch_set_channel(uint8_t channel);
bool sfpiic_read(uint8_t *pData, uint16_t Size);
bool sfpiic_write(uint8_t *pData, uint16_t Size);
bool sfpiic_mem_read(uint16_t addr, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
bool sfpiic_mem_write(uint16_t addr, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
bool sfpiic_get_ch_i2c_status(uint8_t ch);

#ifdef  __cplusplus
}
#endif

#endif // SFPIIC_IIC_DRIVER_H
