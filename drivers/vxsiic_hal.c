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
#include "stm32f7xx_hal_i2c.h"
#include "i2c.h"
#include "bsp_pin_defs.h"
#include "debug_helpers.h"
#include "cmsis_os.h"

static const int I2C_TIMEOUT_MS = 25;

struct __I2C_HandleTypeDef * const vxsiic_hi2c = &hi2c1;

osSemaphoreId vxsiic_semaphore;                         // Semaphore ID
osSemaphoreDef(vxsiic_semaphore);                       // Semaphore definition

void vxsiic_I2C_MasterTxCpltCallback(void)
{
    osSemaphoreRelease(vxsiic_semaphore);
}

void vxsiic_I2C_MasterRxCpltCallback(void)
{
    osSemaphoreRelease(vxsiic_semaphore);
}

void vxsiic_HAL_I2C_MemTxCpltCallback(void)
{
    osSemaphoreRelease(vxsiic_semaphore);
}

void vxsiic_HAL_I2C_MemRxCpltCallback(void)
{
    osSemaphoreRelease(vxsiic_semaphore);
}

void vxsiic_HAL_I2C_ErrorCallback(void)
{
    debug_printf("%s I2C error, code %d\n", __func__, vxsiic_hi2c->ErrorCode);
    // reinitialize I2C
    vxsiic_reset_i2c_master();
    osSemaphoreRelease(vxsiic_semaphore);
}

void vxsiic_HAL_I2C_AbortCpltCallback(void)
{
    debug_printf("%s\n", __func__);
    osSemaphoreRelease(vxsiic_semaphore);
}

enum { PCA9548_BASE_I2C_ADDRESS = 0x71 };
enum {
    PAYLOAD_BOARD_MCU_I2C_ADDRESS = 0x33,
    PAYLOAD_BOARD_EEPROM_I2C_ADDRESS = 0x51
};

void vxsiic_init(void)
{
    // Create and take the semaphore
    vxsiic_semaphore = osSemaphoreCreate(osSemaphore(vxsiic_semaphore), 1);
    osSemaphoreWait(vxsiic_semaphore, osWaitForever);
}

void vxsiic_reset_i2c_master(void)
{
    __HAL_I2C_DISABLE(vxsiic_hi2c);
    vxsiic_hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    vxsiic_hi2c->State = HAL_I2C_STATE_READY;
    vxsiic_hi2c->PreviousState = HAL_I2C_MODE_NONE;
    vxsiic_hi2c->Mode = HAL_I2C_MODE_NONE;
    __HAL_I2C_ENABLE(vxsiic_hi2c);
}

void vxsiic_reset_mux(void)
{
    HAL_GPIO_WritePin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  GPIO_PIN_SET);
}

//void debug_print_hal_error(const char *prefix, uint8_t i2c_address, uint32_t ret)
//{
//    if (ret == HAL_OK)
//        return;
//    if (vxsiic_hi2c->ErrorCode != HAL_I2C_ERROR_TIMEOUT)
//        debug_printf("%s 0x%02X error, HAL code %d, I2C code %d\n", prefix, i2c_address, ret, vxsiic_hi2c->ErrorCode);
//}

//const char * vxs_mux_prefix = "VXS IIC mux";

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
                debug_printf("%s(%d): i2c error %d, %d\n", __func__, i, ret, vxsiic_hi2c->ErrorCode);
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
    uint8_t i2c_address = PCA9548_BASE_I2C_ADDRESS + subdevice;
    ret = HAL_I2C_Master_Transmit_IT(vxsiic_hi2c, i2c_address << 1, &data, 1);
    if (ret != HAL_OK) {
        debug_printf("%s(%d, %d): i2c error %d, %d\n", __func__, subdevice, channel, i2c_address, ret, vxsiic_hi2c->ErrorCode);
    }
    osStatus status = osSemaphoreWait(vxsiic_semaphore, I2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s(%d, %d): i2c timeout\n", __func__, subdevice, channel);
        return HAL_TIMEOUT;
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
//    if (ret != HAL_OK) {
//        debug_printf("%s (port %2d) failed\n", __func__, pp);
//    }
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

//HAL_StatusTypeDef vxsiic_detect_pp_eeprom(uint8_t pp)
//{
//    HAL_StatusTypeDef ret;
//    uint32_t Trials = 2;
//    ret = HAL_I2C_IsDeviceReady(vxsiic_hi2c, PAYLOAD_BOARD_EEPROM_I2C_ADDRESS << 1, Trials, I2C_TIMEOUT_MS);
//    if (ret != HAL_OK) {
//        if (vxsiic_hi2c->ErrorCode == HAL_I2C_ERROR_TIMEOUT)
//            return ret;
//        enum {size = 100};
//        char buf[size];
//        sprint_i2c_error(buf, size, vxsiic_hi2c->ErrorCode);
//        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
//        return ret;
//    }
//    return ret;
//}

HAL_StatusTypeDef vxsiic_read_pp_eeprom(uint8_t pp, uint16_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 1};
    uint8_t pData[Size];
    uint8_t dev_address = (PAYLOAD_BOARD_EEPROM_I2C_ADDRESS << 1) | 1;
    ret = HAL_I2C_Mem_Read_IT(vxsiic_hi2c, dev_address, reg, I2C_MEMADD_SIZE_16BIT, pData, Size);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, empty slot
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (port %d, addr 0x%04X): HAL code %d, I2C code %d\n", __func__, pp, reg, ret, vxsiic_hi2c->ErrorCode);
            return ret;
        }
    }
    osStatus status = osSemaphoreWait(vxsiic_semaphore, I2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (port %2d, read %04X) timeout\n", __func__, pp, reg);
        return HAL_TIMEOUT;
    }
    //    if (ret != HAL_OK) {
    //        if (vxsiic_hi2c->ErrorCode & HAL_I2C_ERROR_AF)
    //            return ret;
    //        enum {size = 100};
    //        char buf[size];
    //        sprint_i2c_error(buf, size, vxsiic_hi2c->ErrorCode);
    //        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    //    }
    //    if (ret == HAL_OK) {
    if (data) {
        *data = pData[0];
    }
    //    }
    //    return ret;
    return HAL_OK;
}

HAL_StatusTypeDef vxsiic_read_pp_mcu_4(uint8_t pp, uint16_t reg, uint32_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 4};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read_IT(vxsiic_hi2c, (PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1) | 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, MCU not loaded
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (port %d, addr 0x%04X): HAL code %d, I2C code %d\n", __func__, pp, reg, ret, vxsiic_hi2c->ErrorCode);
            return ret;
        }
    }
    osStatus status = osSemaphoreWait(vxsiic_semaphore, I2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (port %2d, addr 0x%04X) timeout\n", __func__, pp, reg);
        return HAL_TIMEOUT;
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

HAL_StatusTypeDef vxsiic_write_pp_mcu_4(uint8_t pp, uint16_t reg, uint32_t data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 4};
    uint8_t pData[Size];
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    ret = HAL_I2C_Mem_Write_IT(vxsiic_hi2c, PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, MCU not loaded
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (port %d, addr 0x%04X): HAL code %d, I2C code %d\n", __func__, pp, reg, ret, vxsiic_hi2c->ErrorCode);
            return ret;
        }
    }
    osStatus status = osSemaphoreWait(vxsiic_semaphore, I2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (port %2d, addr 0x%04X) timeout\n", __func__, pp, reg);
        return HAL_TIMEOUT;
    }
    return ret;
}
