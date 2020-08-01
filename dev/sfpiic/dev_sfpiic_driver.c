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
#include "log/logbuffer.h"
#include "i2c.h"

static const int SFPI2C_TIMEOUT_MS = 25;
static const int I2C_TIMEOUT_MS = 10;

void sfpiic_switch_reset(void)
{
//    write_gpio_pin(I2C_RESET3_B_GPIO_Port,  I2C_RESET3_B_Pin,  0);
//    write_gpio_pin(I2C_RESET3_B_GPIO_Port,  I2C_RESET3_B_Pin,  1);
}

bool sfpiic_device_detect(uint16_t addr)
{
    uint32_t Trials = 2;
    return i2c_driver_detect(&hi2c_sfpiic, addr << 1, Trials, I2C_TIMEOUT_MS);
}

bool sfpiic_switch_set_channel(uint8_t channel)
{
    assert(channel < 8);
    uint8_t data = (uint8_t)(1 << channel); // enable channel
    return sfpiic_write(&data, 1);
}

bool sfpiic_read(uint8_t *pData, uint16_t Size)
{
    uint16_t addr = (PCA9548_BASE_I2C_ADDRESS<<1)|1;
    return i2c_driver_read(&hi2c_sfpiic, addr, pData, Size, SFPI2C_TIMEOUT_MS);
}

bool sfpiic_write(uint8_t *pData, uint16_t Size)
{
    uint16_t addr = PCA9548_BASE_I2C_ADDRESS<<1;
    return i2c_driver_write(&hi2c_sfpiic, addr, pData, Size, SFPI2C_TIMEOUT_MS);
}

bool sfpiic_mem_read(uint16_t addr, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    addr = (uint16_t)((addr<<1)|1); // FIXME ???
    return  i2c_driver_mem_read(&hi2c_sfpiic, addr, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, SFPI2C_TIMEOUT_MS);
}

bool sfpiic_mem_read16(uint16_t addr, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    addr = (uint16_t)((addr<<1)|1);
    return i2c_driver_mem_read(&hi2c_sfpiic, addr, MemAddress, I2C_MEMADD_SIZE_16BIT, pData, Size, SFPI2C_TIMEOUT_MS);
}

bool sfpiic_mem_write(uint16_t addr, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    addr = (uint16_t)((addr<<1)|1); // FIXME ???
    return i2c_driver_mem_write(&hi2c_sfpiic, addr, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, SFPI2C_TIMEOUT_MS);
}

bool sfpiic_mem_write16(uint16_t addr, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    addr = (uint16_t)((addr<<1)|1);
    return i2c_driver_mem_write(&hi2c_sfpiic, addr, MemAddress, I2C_MEMADD_SIZE_16BIT, pData, Size, SFPI2C_TIMEOUT_MS);
}

bool sfpiic_get_ch_i2c_status(uint16_t addr)
{
    addr = (uint16_t)((addr<<1)|1);
    int trials = 2;
    return i2c_driver_detect(&hi2c_sfpiic, addr, trials, SFPI2C_TIMEOUT_MS);
}
