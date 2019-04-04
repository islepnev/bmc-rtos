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
#include "vxsiic_hal.h"

#include <stdio.h>
#include "stm32f7xx_hal.h"
#include "i2c.h"
#include "bsp_pin_defs.h"
#include "debug_helpers.h"

static const int I2C_TIMEOUT_MS = 100;

I2C_HandleTypeDef * const vxsiic_hi2c = &hi2c1;

enum { PCA9548_BASE_I2C_ADDRESS = 0x71 };
enum {
    PAYLOAD_BOARD_MCU_I2C_ADDRESS = 0x33,
    PAYLOAD_BOARD_EEPROM_I2C_ADDRESS = 0x51
};

void vxsiic_reset_i2c_master(void)
{
    __HAL_I2C_DISABLE(vxsiic_hi2c);
    __HAL_I2C_ENABLE(vxsiic_hi2c);
}

void vxsiic_reset_mux(void)
{
    HAL_GPIO_WritePin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  GPIO_PIN_SET);
}

HAL_StatusTypeDef vxsiic_detect_mux(void)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    for (int i=0; i<3; i++) {
        vxsiic_reset_i2c_master();
        vxsiic_reset_mux();
        uint8_t i2c_address= PCA9548_BASE_I2C_ADDRESS + i;
        ret = HAL_I2C_IsDeviceReady(vxsiic_hi2c, i2c_address << 1, Trials, I2C_TIMEOUT_MS);
        if (ret != HAL_OK) {
            if (vxsiic_hi2c->ErrorCode != HAL_I2C_ERROR_TIMEOUT)
                debug_printf("VXS IIC mux 0x%02X error %d\n", i2c_address, vxsiic_hi2c->ErrorCode);
            return ret;
        }
    }
    return ret;
}

HAL_StatusTypeDef vxsiic_mux_select(uint8_t subdevice, uint8_t channel)
{
    assert_param(subdevice < 3);
    assert_param(channel < 8);
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t data;
    data = 1 << channel; // enable channel
    ret = HAL_I2C_Master_Transmit(vxsiic_hi2c, (PCA9548_BASE_I2C_ADDRESS + subdevice) << 1, &data, 1, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        debug_printf("%s (%d, %d) failed\n", __func__, subdevice, channel);
    }
    return ret;
}


HAL_StatusTypeDef vxsiic_get_pp_i2c_status(uint8_t pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    HAL_I2C_StateTypeDef state = HAL_I2C_GetState(vxsiic_hi2c);
    if (state != HAL_I2C_STATE_READY) {
        debug_printf("%s (port %2d) I2C not ready: state %d\n", __func__, pp, state);
        ret = HAL_ERROR;
    }
//    uint32_t isr = hi2c->Instance->ISR;
//    debug_printf("ISR = %08X\n", isr);
    if (ret != HAL_OK) {
        debug_printf("%s (port %2d) failed\n", __func__, pp);
    }
    return ret;
}

void sprint_i2c_error(char *buf, size_t size, uint32_t code)
{
    snprintf(buf, size, "%08lX%s%s%s%s%s%s%s",
             code,
             (code &  HAL_I2C_ERROR_BERR) ? " BERR" : "",
             (code &  HAL_I2C_ERROR_ARLO) ? " ARLO" : "",
             (code &  HAL_I2C_ERROR_AF) ? " AF" : "",
             (code &  HAL_I2C_ERROR_OVR) ?  " OVR" : "",
             (code &  HAL_I2C_ERROR_DMA) ?  " DMA" : "",
             (code &  HAL_I2C_ERROR_TIMEOUT) ? " TIMEOUT" : "",
             (code &  HAL_I2C_ERROR_DMA_PARAM) ? " DMA_PARAM" : ""
                                                 );
}

HAL_StatusTypeDef vxsiic_detect_pp_eeprom(uint8_t pp)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    ret = HAL_I2C_IsDeviceReady(vxsiic_hi2c, PAYLOAD_BOARD_EEPROM_I2C_ADDRESS << 1, Trials, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode == HAL_I2C_ERROR_TIMEOUT)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, vxsiic_hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
        return ret;
    }
    return ret;
}

HAL_StatusTypeDef vxsiic_read_pp_eeprom(uint8_t pp, uint16_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 1};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(vxsiic_hi2c, (PAYLOAD_BOARD_EEPROM_I2C_ADDRESS << 1) | 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode == HAL_I2C_ERROR_AF)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, vxsiic_hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    }
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef vxsiic_read_pp_mcu(uint8_t pp, uint16_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 1};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(vxsiic_hi2c, (PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1) | 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode == HAL_I2C_ERROR_AF)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, vxsiic_hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    }
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

HAL_StatusTypeDef vxsiic_read_pp_mcu_4(uint8_t pp, uint16_t reg, uint32_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 4};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(vxsiic_hi2c, (PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1) | 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode == HAL_I2C_ERROR_AF)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, vxsiic_hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    }
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

HAL_StatusTypeDef vxsiic_write_pp_mcu(uint8_t pp, uint16_t reg, uint8_t data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 1};
    uint8_t pData[Size];
    pData[0] = data;
    ret = HAL_I2C_Mem_Write(vxsiic_hi2c, PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode == HAL_I2C_ERROR_AF)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, vxsiic_hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    }
    return ret;
}

HAL_StatusTypeDef vxsiic_write_pp_mcu_4(uint8_t pp, uint16_t reg, uint32_t data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 4};
    uint8_t pData[Size];
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    ret = HAL_I2C_Mem_Write(vxsiic_hi2c, PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode == HAL_I2C_ERROR_AF)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, vxsiic_hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    }
    return ret;
}
