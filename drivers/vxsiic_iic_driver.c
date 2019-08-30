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
#include "vxsiic_iic_driver.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_i2c.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "debug_helpers.h"

struct __I2C_HandleTypeDef * const vxsiic_hi2c = &hi2c1;

static const int I2C_TIMEOUT_MS = 25;

osSemaphoreId vxsiic_semaphore;                         // Semaphore ID
osSemaphoreDef(vxsiic_semaphore);                       // Semaphore definition

void vxsiic_reset_i2c_master(void)
{
    __HAL_I2C_DISABLE(vxsiic_hi2c);
    vxsiic_hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    vxsiic_hi2c->State = HAL_I2C_STATE_READY;
    vxsiic_hi2c->PreviousState = HAL_I2C_MODE_NONE;
    vxsiic_hi2c->Mode = HAL_I2C_MODE_NONE;
    __HAL_I2C_ENABLE(vxsiic_hi2c);
}

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

void vxsiic_init(void)
{
    // Create and take the semaphore
    vxsiic_semaphore = osSemaphoreCreate(osSemaphore(vxsiic_semaphore), 1);
    osSemaphoreWait(vxsiic_semaphore, osWaitForever);
}

HAL_StatusTypeDef vxsiic_read(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_I2C_Master_Receive_IT(vxsiic_hi2c, DevAddress, pData, Size);
    if (ret != HAL_OK) {
        debug_printf("%s(%02X): i2c error %d, %d\n", __func__, DevAddress, ret, vxsiic_hi2c->ErrorCode);
    }
    osStatus status = osSemaphoreWait(vxsiic_semaphore, I2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s(%02X): i2c timeout\n", __func__, DevAddress);
        return HAL_TIMEOUT;
    }
    return ret;
}

HAL_StatusTypeDef vxsiic_write(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_I2C_Master_Transmit_IT(vxsiic_hi2c, DevAddress, pData, Size);
    if (ret != HAL_OK) {
        debug_printf("%s(%02X): i2c error %d, %d\n", __func__, DevAddress, ret, vxsiic_hi2c->ErrorCode);
    }
    osStatus status = osSemaphoreWait(vxsiic_semaphore, I2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s(%02X): i2c timeout\n", __func__, DevAddress);
        return HAL_TIMEOUT;
    }
    return ret;
}

HAL_StatusTypeDef vxsiic_mem_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_I2C_Mem_Read_IT(vxsiic_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, empty slot
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (dev_addr %02X, mem_addr 0x%04X): HAL code %d, I2C code %d\n", __func__, DevAddress, MemAddress, ret, vxsiic_hi2c->ErrorCode);
            return ret;
        }
    }
    osStatus status = osSemaphoreWait(vxsiic_semaphore, I2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (dev_addr %02X, mem_addr 0x%04X) timeout\n", __func__, DevAddress, MemAddress);
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
    //    }
    //    return ret;
    return HAL_OK;
}

HAL_StatusTypeDef vxsiic_mem_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_I2C_Mem_Write_IT(vxsiic_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (vxsiic_hi2c->ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, MCU not loaded
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (dev_addr %02X, mem_addr 0x%04X): HAL code %d, I2C code %d\n", __func__, DevAddress, MemAddress, ret, vxsiic_hi2c->ErrorCode);
            return ret;
        }
    }
    osStatus status = osSemaphoreWait(vxsiic_semaphore, I2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (dev_addr %02X, mem_addr 0x%04X) timeout\n", __func__, DevAddress, MemAddress);
        return HAL_TIMEOUT;
    }
    return HAL_OK;
}
