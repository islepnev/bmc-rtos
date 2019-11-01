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

#include "ad9516_spi_hal.h"

#include "stm32f7xx_hal.h"
#include "gpio.h"
#include "spi.h"
#include "bsp.h"
#include "bsp_pin_defs.h"
#include "error_handler.h"
#include "logbuffer.h"

static const int SPI_TIMEOUT_MS = 500;

void set_csb(int state)
{
    GPIO_PinState write = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(AD9516_CS_B_GPIO_Port, AD9516_CS_B_Pin, write);
    GPIO_PinState read = HAL_GPIO_ReadPin(AD9516_CS_B_GPIO_Port, AD9516_CS_B_Pin);
    if (write != read) {
        Error_Handler();
    }
}

HAL_StatusTypeDef ad9516_read1(uint16_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 3};
    uint8_t tx[Size];
    uint8_t rx[Size];
    reg &= 0x1FFF;
    tx[0] = 0x80 | (reg >> 8);  // MSB first, bit 15 = read
    tx[1] = reg & 0xFF;
    tx[2] = 0;
    set_csb(0);
    ret = HAL_SPI_TransmitReceive(ad9516_spi, tx, rx, Size, SPI_TIMEOUT_MS);
    set_csb(1);
    if (ret == HAL_OK) {
        if (data) {
            *data = rx[2];
        }
        if (*data != 0xC3) {
            *data = 0;
        }
    } else {
        Error_Handler();
    }
    return ret;
}

HAL_StatusTypeDef ad9516_write1(uint16_t reg, uint8_t data)
{
    HAL_StatusTypeDef ret;
    enum {Size = 3};
    uint8_t tx[Size];
    reg &= 0x1FFF;
    tx[0] = (reg >> 8) & 0x1F;  // MSB first
    tx[1] = reg & 0xFF;
    tx[2] = data;
    set_csb(0);
    ret = HAL_SPI_Transmit(ad9516_spi, tx, Size, SPI_TIMEOUT_MS);
    set_csb(1);
    uint8_t readback = 0;
    ad9516_read1(reg, &readback);
    if (readback != data) {
        log_printf(LOG_ERR, "Error writing %04X: wrote %02X, read %02X", reg, data, readback);
        return HAL_ERROR;
    }
    return ret;
}
/*
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
*/
