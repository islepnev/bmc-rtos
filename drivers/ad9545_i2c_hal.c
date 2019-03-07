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

#include "ad9545_i2c_hal.h"

#include "stm32f7xx_hal.h"
#include "i2c.h"

I2C_HandleTypeDef * const hPll = &hi2c2;

static const int pllDeviceAddr = 0x4A;
static const int I2C_TIMEOUT_MS = 100;

void pllSetStaticPins(void)
{
    //    HAL_GPIO_WritePin(PLL_M0_GPIO_Port, PLL_M0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PLL_M3_GPIO_Port, PLL_M3_Pin, GPIO_PIN_RESET); // M3=0 - do not load eeprom
    HAL_GPIO_WritePin(PLL_M4_GPIO_Port, PLL_M4_Pin, GPIO_PIN_SET);   // M4=1 - I2C mode
    HAL_GPIO_WritePin(PLL_M5_GPIO_Port, PLL_M5_Pin, GPIO_PIN_RESET); // M5=0 - I2C address offset
    HAL_GPIO_WritePin(PLL_M6_GPIO_Port, PLL_M6_Pin, GPIO_PIN_SET);   // M6=1 - I2C address offset
}

HAL_StatusTypeDef ad9545_detect(void)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    ret = HAL_I2C_IsDeviceReady(hPll, pllDeviceAddr << 1, Trials, I2C_TIMEOUT_MS);
    return ret;
}

static HAL_StatusTypeDef pllSendByte(uint16_t data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 2};
    uint8_t pData[Size];
    pData[0] = (data >> 8) & 0xFF;
    pData[1] = data & 0xFF;
    ret = HAL_I2C_Master_Transmit(hPll, pllDeviceAddr << 1, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}

static HAL_StatusTypeDef pllReceiveByte(uint32_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 3};
    uint8_t pData[Size] = {0, 0, 0};
    ret = HAL_I2C_Master_Receive(hPll, pllDeviceAddr << 1, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = ((uint32_t)pData[2] << 16) | ((uint32_t)pData[1] << 8) | pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef ad9545_read1(uint16_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 1};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hPll, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef ad9545_write1(uint16_t reg, uint8_t data)
{
    HAL_StatusTypeDef ret;
    int Size = 1;
    uint8_t pData[Size];
    pData[0] = data & 0xFF;
    ret = HAL_I2C_Mem_Write(hPll, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef ad9545_read2(uint16_t reg, uint16_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 2};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hPll, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = ((uint32_t)pData[1] << 8) | pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef ad9545_write2(uint16_t reg, uint16_t data)
{
    HAL_StatusTypeDef ret;
    int Size = 2;
    uint8_t pData[Size];
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    ret = HAL_I2C_Mem_Write(hPll, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef ad9545_read3(uint16_t reg, uint32_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 3};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hPll, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = ((uint32_t)pData[2] << 16) | ((uint32_t)pData[1] << 8) | pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef ad9545_write3(uint16_t reg, uint32_t data)
{
    HAL_StatusTypeDef ret;
    int Size = 3;
    uint8_t pData[Size];
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    ret = HAL_I2C_Mem_Write(hPll, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef ad9545_read4(uint16_t reg, uint32_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 4};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hPll, pllDeviceAddr << 1, reg , I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = ((uint32_t)pData[3] << 24)
                    | ((uint32_t)pData[2] << 16)
                    | ((uint32_t)pData[1] << 8)
                    | pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef ad9545_write4(uint16_t reg, uint32_t data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 4};
    uint8_t pData[Size];
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    ret = HAL_I2C_Mem_Write(hPll, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef ad9545_read5(uint16_t reg, uint64_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 5};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hPll, pllDeviceAddr << 1, reg , I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = ((uint64_t)pData[4] << 32)
                    | ((uint32_t)pData[3] << 24)
                    | ((uint32_t)pData[2] << 16)
                    | ((uint16_t)pData[1] << 8)
                    | pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef ad9545_write5(uint16_t reg, uint64_t data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 5};
    uint8_t pData[Size];
    pData[4] = (data >> 32) & 0xFF;
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    ret = HAL_I2C_Mem_Write(hPll, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef ad9545_read6(uint16_t reg, uint64_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 6};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hPll, pllDeviceAddr << 1, reg , I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret == HAL_OK) {
        if (data) {
            *data = ((uint64_t)pData[5] << 40)
                    | ((uint64_t)pData[4] << 32)
                    | ((uint32_t)pData[3] << 24)
                    | ((uint32_t)pData[2] << 16)
                    | ((uint16_t)pData[1] << 8)
                    | pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef ad9545_write6(uint16_t reg, uint64_t data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 6};
    uint8_t pData[Size];
    pData[5] = (data >> 40) & 0xFF;
    pData[4] = (data >> 32) & 0xFF;
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    ret = HAL_I2C_Mem_Write(hPll, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef ad9545_write8(uint16_t reg, uint64_t data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 8};
    uint8_t pData[Size];
    pData[7] = (data >> 56) & 0xFF;
    pData[6] = (data >> 48) & 0xFF;
    pData[5] = (data >> 40) & 0xFF;
    pData[4] = (data >> 32) & 0xFF;
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    ret = HAL_I2C_Mem_Write(hPll, pllDeviceAddr << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}
