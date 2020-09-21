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

#include "i2c_driver.h"

#include <assert.h>

#include "bus_types.h"
#include "cmsis_os.h"
#include "error_handler.h"
#include "i2c.h"
#include "impl/i2c_driver_impl.h"
#include "impl/i2c_driver_util.h"
#include "log/log.h"
#include "stm32_hal.h"

void i2c_driver_init(void)
{
    if (! i2c_driver_util_init())
        Error_Handler();
}

bool i2c_driver_get_master_ready(BusInterface *bus)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    return  HAL_I2C_STATE_READY == HAL_I2C_GetState(hi2c);
}

// Device-locked functions

bool i2c_driver_bus_ready(BusInterface *bus)
{
    if (osOK != i2c_driver_wait_dev_mutex(bus->bus_number, osWaitForever))
        return false;
    bool result = i2c_driver_bus_ready_internal(bus);
    i2c_driver_release_dev_mutex(bus->bus_number);
    return result;
}

bool i2c_driver_detect(BusInterface *bus, uint32_t Trials, uint32_t millisec)
{
    if (osOK != i2c_driver_wait_dev_mutex(bus->bus_number, osWaitForever))
        return false;
    bool result =  i2c_driver_detect_internal(bus, Trials, millisec);
    i2c_driver_release_dev_mutex(bus->bus_number);
    return result;
}

bool i2c_driver_read(BusInterface *bus, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    if (osOK != i2c_driver_wait_dev_mutex(bus->bus_number, osWaitForever))
        return false;
    bool ret = i2c_driver_read_internal(bus, pData, Size, millisec);
    i2c_driver_release_dev_mutex(bus->bus_number);
    return ret;
}

bool i2c_driver_write(BusInterface *bus, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    if (osOK != i2c_driver_wait_dev_mutex(bus->bus_number, osWaitForever))
        return false;
    bool ret = i2c_driver_write_internal(bus, pData, Size, millisec);
    i2c_driver_release_dev_mutex(bus->bus_number);
    return ret;
}

static bool i2c_driver_mem_read(BusInterface *bus, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    if (osOK != i2c_driver_wait_dev_mutex(bus->bus_number, osWaitForever))
        return false;
    bool ret = i2c_driver_mem_read_internal(bus, MemAddress, MemAddSize, pData, Size, millisec);
    i2c_driver_release_dev_mutex(bus->bus_number);
    return ret;
}

static bool i2c_driver_mem_write(BusInterface *bus, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    if (osOK != i2c_driver_wait_dev_mutex(bus->bus_number, osWaitForever))
        return false;
    bool ret = i2c_driver_mem_write_internal(bus, MemAddress, MemAddSize, pData, Size, millisec);
    i2c_driver_release_dev_mutex(bus->bus_number);
    return ret;
}

bool i2c_driver_mem_read8(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    return i2c_driver_mem_read(bus, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, millisec);
}

bool i2c_driver_mem_read16(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    return i2c_driver_mem_read(bus, MemAddress, I2C_MEMADD_SIZE_16BIT, pData, Size, millisec);
}

bool i2c_driver_mem_write8(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    return i2c_driver_mem_write(bus, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, millisec);
}

bool i2c_driver_mem_write16(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    return i2c_driver_mem_write(bus, MemAddress, I2C_MEMADD_SIZE_16BIT, pData, Size, millisec);
}
