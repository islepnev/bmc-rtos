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

#include "bus_types.h"
#include "cmsis_os.h"
#include "log/logbuffer.h"
#include "error_handler.h"
#include "i2c.h"
#include "i2c_driver_util.h"


void i2c_driver_init(void)
{
    if (! i2c_driver_util_init())
        Error_Handler();
}

void i2c_driver_reset(struct __I2C_HandleTypeDef *hi2c)
{
    __HAL_I2C_DISABLE(hi2c);
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->PreviousState = HAL_I2C_MODE_NONE;
    hi2c->Mode = HAL_I2C_MODE_NONE;
    __HAL_I2C_ENABLE(hi2c);
}

bool i2c_driver_get_master_ready(struct __I2C_HandleTypeDef *hi2c)
{
    return  HAL_I2C_STATE_READY == HAL_I2C_GetState(&hi2c);
}

void HAL_I2C_MasterTxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    release_it_sem(hi2c);
}

void HAL_I2C_MasterRxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    release_it_sem(hi2c);
}

void HAL_I2C_MemTxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    release_it_sem(hi2c);
}

void HAL_I2C_MemRxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    release_it_sem(hi2c);
}

void HAL_I2C_ErrorCallback(struct __I2C_HandleTypeDef *hi2c)
{
    log_printf(LOG_WARNING, "%s I2C%d error, code %d\n", __func__, hi2c_index(hi2c), hi2c->ErrorCode);
    // reinitialize I2C
    i2c_driver_reset(hi2c);
    release_it_sem(hi2c);
}

void HAL_I2C_AbortCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    log_printf(LOG_WARNING, "%s I2C%d\n", __func__, hi2c_index(hi2c));
    release_it_sem(hi2c);
}

bool i2c_driver_detect(struct __I2C_HandleTypeDef *hi2c, uint16_t deviceAddr, uint32_t Trials, uint32_t millisec)
{
    return HAL_OK == HAL_I2C_IsDeviceReady(hi2c, deviceAddr, Trials, millisec);
}

bool i2c_driver_read(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    HAL_StatusTypeDef ret = HAL_I2C_Master_Receive_IT(hi2c, DevAddress, pData, Size);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode & (HAL_I2C_ERROR_AF | HAL_I2C_ERROR_TIMEOUT)) {
            return false; // no acknowledge or timeout
        }
        log_printf(LOG_WARNING, "%s (%02X): I2C%d error %d, %d\n", __func__, DevAddress, hi2c_index(hi2c), ret, hi2c->ErrorCode);
        return false;
    }
    int32_t status = wait_it_sem(hi2c, millisec);
    if (status != osOK) {
        log_printf(LOG_WARNING, "%s (%02X): I2C%d timeout\n", __func__, hi2c_index(hi2c), DevAddress);
        return false;
    }
    return true;
}

bool i2c_driver_write(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit_IT(hi2c, DevAddress, pData, Size);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode & (HAL_I2C_ERROR_AF | HAL_I2C_ERROR_TIMEOUT)) {
            return false; // no acknowledge or timeout
        }
        log_printf(LOG_WARNING, "%s(%02X): I2C%d error %d, %d\n", __func__, DevAddress, hi2c_index(hi2c), ret, hi2c->ErrorCode);
        return false;
    }
    int32_t status = wait_it_sem(hi2c, millisec);
    if (status != osOK) {
        log_printf(LOG_WARNING, "%s (%02X): I2C%d timeout\n", __func__, hi2c_index(hi2c), DevAddress);
        return false;
    }
    return true;
}

bool i2c_driver_mem_read(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode & (HAL_I2C_ERROR_AF | HAL_I2C_ERROR_TIMEOUT)) {
            return false; // no acknowledge or timeout
        }
        log_printf(LOG_WARNING, "%s (%02X, 0x%04X): I2C%d error %d, I2C code %d\n",
                     __func__, DevAddress, MemAddress, hi2c_index(hi2c), ret, hi2c->ErrorCode);
        return false;
    }
    int32_t status = wait_it_sem(hi2c, millisec);
    if (status != osOK) {
        log_printf(LOG_WARNING, "%s (%02X, 0x%04X) I2C%d timeout\n", __func__, DevAddress, MemAddress, hi2c_index(hi2c));
        return false;
    }
    return true;
}

bool i2c_driver_mem_write(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode & (HAL_I2C_ERROR_AF | HAL_I2C_ERROR_TIMEOUT)) {
            return false; // no acknowledge or timeout
        }
        log_printf(LOG_WARNING, "%s (%02X, 0x%04X): I2C%d error %d, I2C code %d\n",
                     __func__, DevAddress, MemAddress, hi2c_index(hi2c), ret, hi2c->ErrorCode);
        return false;

    }
    int32_t status = wait_it_sem(hi2c, millisec);
    if (status != osOK) {
        log_printf(LOG_WARNING, "%s (%02X, 0x%04X) I2C%d timeout\n", __func__, DevAddress, MemAddress, hi2c_index(hi2c));
        return false;
    }
    return true;
}
