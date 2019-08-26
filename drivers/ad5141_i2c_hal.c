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
#include "ad5141_i2c_hal.h"

#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_i2c.h"
#include "bsp.h"
#include "powermon_i2c_driver.h"

static HAL_StatusTypeDef ad5141_write(uint8_t deviceAddress, uint8_t ctrl_addr, uint8_t data)
{
    return powermon_i2c_mem_write(deviceAddress << 1, ctrl_addr, I2C_MEMADD_SIZE_8BIT, &data, 1);
}

static HAL_StatusTypeDef ad5141_read(uint8_t deviceAddress, uint16_t command, uint8_t *data)
{
    return powermon_i2c_mem_read(deviceAddress << 1, command, I2C_MEMADD_SIZE_16BIT, data, 1);
}

HAL_StatusTypeDef ad5141_nop(uint8_t deviceAddress)
{
    uint8_t ctrl_addr = 0x00;
    return ad5141_write(deviceAddress, ctrl_addr, 0);
}

HAL_StatusTypeDef ad5141_reset(uint8_t deviceAddress)
{
    uint8_t ctrl_addr = 0xB0;
    return ad5141_write(deviceAddress, ctrl_addr, 0);
}

HAL_StatusTypeDef ad5141_copy_rdac_to_eeprom(uint8_t deviceAddress)
{
    uint8_t ctrl_addr = 0x70;
    uint8_t data = 0x01;
    HAL_StatusTypeDef ret = ad5141_write(deviceAddress, ctrl_addr, data);
    // wait 18 ms
    for (int i=0; i<360; i++)
        ad5141_nop(deviceAddress);
    return ret;
}

HAL_StatusTypeDef ad5141_copy_eeprom_to_rdac(uint8_t deviceAddress)
{
    uint8_t ctrl_addr = 0x70;
    uint8_t data = 0x00;
    return ad5141_write(deviceAddress, ctrl_addr, data);
}

HAL_StatusTypeDef ad5141_write_rdac(uint8_t deviceAddress, uint8_t data)
{
    uint8_t ctrl_addr = 0x10;
    return ad5141_write(deviceAddress, ctrl_addr, data);
}

HAL_StatusTypeDef ad5141_read_rdac(uint8_t deviceAddress, uint8_t *data)
{
    uint16_t command = 0x3003;
    return ad5141_read(deviceAddress, command, data);
}

HAL_StatusTypeDef ad5141_inc_rdac(uint8_t deviceAddress)
{
    uint8_t ctrl_addr = 0x40;
    return ad5141_write(deviceAddress, ctrl_addr, 1);
}

HAL_StatusTypeDef ad5141_dec_rdac(uint8_t deviceAddress)
{
    uint8_t ctrl_addr = 0x40;
    return ad5141_write(deviceAddress, ctrl_addr, 0);
}
