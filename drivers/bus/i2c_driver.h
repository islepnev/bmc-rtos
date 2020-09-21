/*
**    Generic interrupt mode IIC driver
**
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

#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#include "bus/bus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void i2c_driver_init(void);

bool i2c_driver_get_master_ready(BusInterface *bus)
    __attribute__((warn_unused_result));

bool i2c_driver_bus_ready(BusInterface *bus)
    __attribute__((warn_unused_result));

bool i2c_driver_detect(BusInterface *bus,
                       uint32_t Trials, uint32_t millisec)
    __attribute__((warn_unused_result));

bool i2c_driver_read(BusInterface *bus,
                     uint8_t *pData, uint16_t Size, uint32_t millisec)
    __attribute__((warn_unused_result));

bool i2c_driver_write(BusInterface *bus,
                      uint8_t *pData, uint16_t Size, uint32_t millisec)
    __attribute__((warn_unused_result));

//bool i2c_driver_mem_read(BusInterface *bus,
//                         uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
//    __attribute__((warn_unused_result));

//bool i2c_driver_mem_write(BusInterface *bus,
//                          uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
//    __attribute__((warn_unused_result));

bool i2c_driver_mem_read8(BusInterface *bus,
                           uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
    __attribute__((warn_unused_result));

bool i2c_driver_mem_read16(BusInterface *bus,
                            uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
    __attribute__((warn_unused_result));

bool i2c_driver_mem_write8(BusInterface *bus,
                           uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
    __attribute__((warn_unused_result));

bool i2c_driver_mem_write16(BusInterface *bus,
                            uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
    __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif // I2C_DRIVER_H
