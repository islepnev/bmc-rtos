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

#include "mcp23017_i2c_hal.h"

#include "stm32f7xx_hal.h"
#include "i2c.h"

static I2C_HandleTypeDef * const hi2c = &hi2c4;

static const int I2C_TIMEOUT_MS = 10;

enum { MCP23017_BASE_I2C_ADDRESS = 0x20 };

HAL_StatusTypeDef mcp23017_detect(void)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    ret = HAL_I2C_IsDeviceReady(hi2c, MCP23017_BASE_I2C_ADDRESS << 1, Trials, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef mcp23017_read(uint8_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 1};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hi2c, MCP23017_BASE_I2C_ADDRESS << 1, reg, I2C_MEMADD_SIZE_8BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef mcp23017_write(uint8_t reg, uint8_t data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 1};
    uint8_t pData[Size];
    pData[0] = data;
    ret = HAL_I2C_Mem_Write(hi2c, MCP23017_BASE_I2C_ADDRESS << 1, reg, I2C_MEMADD_SIZE_8BIT, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}
