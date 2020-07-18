//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef INA226_I2C_HAL_H
#define INA226_I2C_HAL_H

#include <stdint.h>
#include "stm32f7xx_hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {INA226_MANUFACTURER_ID = 0x5449};
enum {INA226_DEVICE_ID = 0x2260};
enum {
    INA226_REG_CONFIG = 0x00,
    INA226_REG_SHUNT_VOLT = 0x01,
    INA226_REG_BUS_VOLT = 0x02,
    INA226_REG_POWER = 0x03,
    INA226_REG_CURRENT = 0x04,
    INA226_REG_CAL = 0x05,
    INA226_REG_MASK = 0x06,
    INA226_REG_ALERT = 0x07,
    INA226_REG_MANUFACTURER_ID = 0xFE,
    INA226_REG_DEVICE_ID = 0xFF
};

HAL_StatusTypeDef ina226_i2c_Detect(uint16_t deviceAddr);
HAL_StatusTypeDef ina226_i2c_Read(uint16_t deviceAddr, uint16_t reg, uint16_t *data);
HAL_StatusTypeDef ina226_i2c_Write(uint16_t deviceAddr, uint16_t reg, uint16_t data);

#ifdef __cplusplus
}
#endif

#endif // INA226_I2C_HAL_H
