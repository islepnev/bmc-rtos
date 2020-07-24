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

#include "dev_eeprom.h"
#include "dev_eeprom_types.h"
#include "bsp.h"
#include "i2c.h"
#include "bus/i2c_driver.h"

static const int I2C_TIMEOUT_MS = 10;

static void struct_at24c_init(Dev_at24c *d)
{
    d->present = DEVICE_UNKNOWN;
}

static bool at24c_detect(uint8_t address)
{
    uint32_t Trials = 10;
    return i2c_driver_detect(&hi2c_eeprom_cfg, address << 1, Trials, I2C_TIMEOUT_MS);
}

bool at24c_read(uint8_t address, uint16_t addr, uint8_t *data)
{
    enum {Size = 1};
    uint8_t pData[Size];
    bool ret = i2c_driver_mem_read(&hi2c_eeprom_cfg, address << 1, addr, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

DeviceStatus dev_eepromConfig_detect(Dev_at24c *d)
{
    struct_at24c_init(d);
    if (at24c_detect(eeprom_cfg_deviceAddr))
        d->present = DEVICE_NORMAL;
    else
        d->present = DEVICE_FAIL;
//    uint8_t data = 0;
//    if (at24c_read(eeprom_Config_busAddress, 0, &data)) {
//        d->present = DEVICE_NORMAL;
//    }
    return d->present;
}

DeviceStatus dev_eepromConfig_read(Dev_at24c *d)
{
    uint8_t data = 0;
    if (! at24c_read(eeprom_cfg_deviceAddr, 0, &data)) {
        d->present = DEVICE_FAIL;
    }
    return d->present;
}

