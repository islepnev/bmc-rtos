/*
**    Generic interrupt mode SPI driver
**
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

#include "i2c_driver.h"

#include "i2c.h"
#include "debug_helpers.h"
#include "cmsis_os.h"
#include "error_handler.h"

osSemaphoreId i2c1_sem = NULL;
osSemaphoreId i2c2_sem = NULL;
osSemaphoreId i2c3_sem = NULL;
osSemaphoreId i2c4_sem = NULL;
osSemaphoreDef(i2c1_sem);
osSemaphoreDef(i2c2_sem);
osSemaphoreDef(i2c3_sem);
osSemaphoreDef(i2c4_sem);

void i2c_driver_init(void)
{
    // Create and take the semaphore
    i2c1_sem = osSemaphoreCreate(osSemaphore(i2c1_sem), 1);
    i2c2_sem = osSemaphoreCreate(osSemaphore(i2c2_sem), 1);
    i2c3_sem = osSemaphoreCreate(osSemaphore(i2c3_sem), 1);
    i2c4_sem = osSemaphoreCreate(osSemaphore(i2c4_sem), 1);
    if (NULL == i2c1_sem
            || NULL == i2c2_sem
            || NULL == i2c3_sem
            || NULL == i2c4_sem
            ) {
        Error_Handler();
    }
    osSemaphoreWait(i2c1_sem, osWaitForever);
    osSemaphoreWait(i2c2_sem, osWaitForever);
    osSemaphoreWait(i2c3_sem, osWaitForever);
    osSemaphoreWait(i2c4_sem, osWaitForever);
}

void i2c_driver_reset(struct __I2C_HandleTypeDef *handle)
{
    __HAL_I2C_DISABLE(handle);
    handle->ErrorCode = HAL_I2C_ERROR_NONE;
    handle->State = HAL_I2C_STATE_READY;
    handle->PreviousState = HAL_I2C_MODE_NONE;
    handle->Mode = HAL_I2C_MODE_NONE;
    __HAL_I2C_ENABLE(handle);
}

static SemaphoreHandle_t sem_by_hi2c(struct __I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1)
        return i2c1_sem;
    if (hi2c == &hi2c2)
        return i2c2_sem;
    if (hi2c == &hi2c3)
        return i2c3_sem;
    if (hi2c == &hi2c4)
        return i2c4_sem;
    return NULL;
}

static int32_t i2c_driver_wait_sem(struct __I2C_HandleTypeDef *hi2c, uint32_t millisec)
{
    SemaphoreHandle_t sem = sem_by_hi2c(hi2c);
    if (sem)
        return osSemaphoreWait(sem, millisec);
    else
        return -1;
}

void i2c_driver_release_sem(struct __I2C_HandleTypeDef *hi2c)
{
    SemaphoreHandle_t sem = sem_by_hi2c(hi2c);
    if (sem)
        osSemaphoreRelease(sem);
}

void HAL_I2C_MasterTxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    i2c_driver_release_sem(hi2c);
}

void HAL_I2C_MasterRxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    i2c_driver_release_sem(hi2c);
}

void HAL_I2C_MemTxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    i2c_driver_release_sem(hi2c);
}

void HAL_I2C_MemRxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    i2c_driver_release_sem(hi2c);
}

void HAL_I2C_ErrorCallback(struct __I2C_HandleTypeDef *hi2c)
{
    debug_printf("%s I2C error, code %d\n", __func__, hi2c->ErrorCode);
    // reinitialize I2C
    i2c_driver_reset(hi2c);
    i2c_driver_release_sem(hi2c);
}

void HAL_I2C_AbortCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    debug_printf("%s\n", __func__);
    i2c_driver_release_sem(hi2c);
}

HAL_StatusTypeDef i2c_driver_detect(struct __I2C_HandleTypeDef *hi2c, uint16_t deviceAddr, uint32_t Trials, uint32_t millisec)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_IsDeviceReady(hi2c, deviceAddr, Trials, millisec);
    return ret;
}

HAL_StatusTypeDef i2c_driver_read(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{

    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_I2C_Master_Receive_IT(hi2c, DevAddress, pData, Size);
    if (ret != HAL_OK) {
        debug_printf("%s (%02X): i2c error %d, %d\n", __func__, DevAddress, ret, hi2c->ErrorCode);
    }
    osStatus status = i2c_driver_wait_sem(hi2c, millisec);
    if (status != osOK) {
        debug_printf("%s (%02X): i2c timeout\n", __func__, DevAddress);
        return HAL_TIMEOUT;
    }
    return ret;
}

HAL_StatusTypeDef i2c_driver_write(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_I2C_Master_Transmit_IT(hi2c, DevAddress, pData, Size);
    if (ret != HAL_OK) {
        debug_printf("%s(%02X): i2c error %d, %d\n", __func__, DevAddress, ret, hi2c->ErrorCode);
    }
    osStatus status = i2c_driver_wait_sem(hi2c, millisec);
    if (status != osOK) {
        debug_printf("%s(%02X): i2c timeout\n", __func__, DevAddress);
        return HAL_TIMEOUT;
    }
    return ret;
}

HAL_StatusTypeDef i2c_driver_mem_read(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_Mem_Read_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, empty slot
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (%02X, 0x%04X): HAL code %d, I2C code %d\n",
                         __func__, DevAddress, MemAddress, ret, hi2c->ErrorCode);
            return ret;
        }
    }
    osStatus status = i2c_driver_wait_sem(hi2c, millisec);
    if (status != osOK) {
        debug_printf("%s (%02X, 0x%04X) timeout\n", __func__, DevAddress, MemAddress);
        return HAL_TIMEOUT;
    }
    return HAL_OK;
}

HAL_StatusTypeDef i2c_driver_mem_write(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_Mem_Write_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, empty slot
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (%02X, 0x%04X): HAL code %d, I2C code %d\n",
                         __func__, DevAddress, MemAddress, ret, hi2c->ErrorCode);
            return ret;
        }
    }
    osStatus status = i2c_driver_wait_sem(hi2c, millisec);
    if (status != osOK) {
        debug_printf("%s (%02X, 0x%04X) timeout\n", __func__, DevAddress, MemAddress);
        return HAL_TIMEOUT;
    }
    return HAL_OK;
}
