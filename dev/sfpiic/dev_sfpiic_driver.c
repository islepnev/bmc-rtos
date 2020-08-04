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

#include "dev_sfpiic_driver.h"

#include <assert.h>

#include "bsp.h"
#include "bsp_pin_defs.h"
#include "bus/i2c_driver.h"
#include "bus/impl/i2c_driver_util.h" // FIXME: use index, not handle
#include "dev_sfpiic_types.h"
#include "i2c.h"
#include "log/log.h"

static const int SFPI2C_TIMEOUT_MS = 25;
static const int I2C_TIMEOUT_MS = 10;

bool sfpiic_pca9548_detect(Dev_pca9548 *p)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(p->dev.bus.bus_number);
    uint16_t DevAddress = p->dev.bus.address << 1;
    uint32_t Trials = 2;
    bool ret = i2c_driver_detect(hi2c, DevAddress, Trials, I2C_TIMEOUT_MS);
    p->dev.device_status = ret ? DEVICE_NORMAL : DEVICE_UNKNOWN;
    return ret;
}

static bool sfpiic_pca9548_read(BusInterface *bus, uint8_t *pData, uint16_t Size)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    uint16_t DevAddress = bus->address << 1;
    return i2c_driver_read(hi2c, DevAddress, pData, Size, SFPI2C_TIMEOUT_MS);
}

static bool sfpiic_pca9548_write(BusInterface *bus, uint8_t *pData, uint16_t Size)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    uint16_t DevAddress = bus->address << 1;
    return i2c_driver_write(hi2c, DevAddress, pData, Size, SFPI2C_TIMEOUT_MS);
}

bool sfpiic_pca9548_set_channel(Dev_pca9548 *p, uint8_t channel, bool enable)
{
    assert(channel < 8);
    uint8_t data = enable ? (uint8_t)(1 << channel) : 0; // enable channel
    bool ret = sfpiic_pca9548_write(&p->dev.bus, &data, 1);
    p->dev.device_status = ret ? DEVICE_NORMAL : DEVICE_FAIL;
    return ret;
}

bool sfpiic_mem_read(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    uint16_t DevAddress = bus->address << 1;
    return  i2c_driver_mem_read(hi2c, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, SFPI2C_TIMEOUT_MS);
}

bool sfpiic_mem_write(BusInterface *bus, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    uint16_t DevAddress = bus->address << 1;
    return i2c_driver_mem_write(hi2c, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, SFPI2C_TIMEOUT_MS);
}

bool sfpiic_get_ch_i2c_status(BusInterface *bus)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    uint16_t DevAddress = bus->address << 1;
    int trials = 2;
    return i2c_driver_detect(hi2c, DevAddress, trials, SFPI2C_TIMEOUT_MS);
}
