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

#include "i2c_driver_impl.h"

#include "assert.h"

#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "log/logbuffer.h"
#include "error_handler.h"
#include "i2c.h"
#include "i2c_driver_util.h"

void i2c_driver_reset_internal(struct __I2C_HandleTypeDef *hi2c)
{
    __HAL_I2C_DISABLE(hi2c);
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->PreviousState = HAL_I2C_MODE_NONE;
    hi2c->Mode = HAL_I2C_MODE_NONE;
    __HAL_I2C_ENABLE(hi2c);
}

bool i2c_driver_detect_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t deviceAddr, uint32_t Trials, uint32_t millisec)
{
    i2c_driver_reset_internal(hi2c);
    HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(hi2c, deviceAddr, Trials, millisec);
    if (ret != HAL_OK) {
        assert(ret != HAL_BUSY);
        if (hi2c->ErrorCode & (HAL_I2C_ERROR_AF | HAL_I2C_ERROR_TIMEOUT)) {
            log_printf(LOG_WARNING, "%s: I2C %d.%02X timeout (error code 0x%04X)\n",
                       __func__, hi2c_index(hi2c), deviceAddr, hi2c->ErrorCode);
            return false;
        }
        log_printf(LOG_WARNING, "%s: I2C %d.%02X error %d (error code 0x%04X)\n",
                   __func__, hi2c_index(hi2c), deviceAddr, ret, hi2c->ErrorCode);
        return false;
    }
    return true;
}

bool i2c_driver_read_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    HAL_StatusTypeDef ret = HAL_I2C_Master_Receive_IT(hi2c, DevAddress, pData, Size);
    if (ret != HAL_OK) {
        assert(ret != HAL_BUSY);
        if (hi2c->ErrorCode & (HAL_I2C_ERROR_AF | HAL_I2C_ERROR_TIMEOUT)) {
            log_printf(LOG_WARNING, "%s: I2C %d.%02X timeout (error code 0x%04X)\n",
                       __func__, hi2c_index(hi2c), DevAddress, hi2c->ErrorCode);
            return false; // no acknowledge or timeout
        }
        log_printf(LOG_WARNING, "%s: I2C %d.%02X error %d (error code 0x%04X)\n",
                   __func__, hi2c_index(hi2c), DevAddress, ret, hi2c->ErrorCode);
        return false;
    }
    int32_t status = wait_it_sem(hi2c, millisec);
    if (status != osOK) {
        log_printf(LOG_WARNING, "%s: I2C %d.%02X timeout\n", __func__, hi2c_index(hi2c), DevAddress);
        HAL_I2C_Master_Abort_IT(hi2c, DevAddress);
        i2c_driver_reset_internal(hi2c);
        return false;
    }
    return true;
}

bool i2c_driver_write_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit_IT(hi2c, DevAddress, pData, Size);
    if (ret != HAL_OK) {
        assert(ret != HAL_BUSY);
        if (hi2c->ErrorCode & (HAL_I2C_ERROR_AF | HAL_I2C_ERROR_TIMEOUT)) {
            log_printf(LOG_WARNING, "%s: I2C %d.%02X timeout (error code 0x%04X)\n",
                       __func__, hi2c_index(hi2c), DevAddress, hi2c->ErrorCode);
            return false; // no acknowledge or timeout
        }
        log_printf(LOG_WARNING, "%s: I2C %d.%02X error %d (error code 0x%04X)\n",
                   __func__, hi2c_index(hi2c), DevAddress, ret, hi2c->ErrorCode);
        return false;
    }
    int32_t status = wait_it_sem(hi2c, millisec);
    if (status != osOK) {
        log_printf(LOG_WARNING, "%s: I2C %d.%02X timeout\n", __func__, hi2c_index(hi2c), DevAddress);
        HAL_I2C_Master_Abort_IT(hi2c, DevAddress);
        i2c_driver_reset_internal(hi2c);
        return false;
    }
    return true;
}

bool i2c_driver_mem_read_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    i2c_driver_reset_internal(hi2c);
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        assert(ret != HAL_BUSY);
        if (hi2c->ErrorCode & (HAL_I2C_ERROR_AF | HAL_I2C_ERROR_TIMEOUT)) {
            log_printf(LOG_WARNING, "%s (0x%04X): I2C %d.%02X timeout (error code 0x%04X)\n",
                       __func__, MemAddress, hi2c_index(hi2c), DevAddress, hi2c->ErrorCode);
//            HAL_I2C_Master_Abort_IT(hi2c, DevAddress);
            i2c_driver_reset_internal(hi2c);
            return false; // no acknowledge or timeout
        }
        log_printf(LOG_WARNING, "%s (0x%04X): I2C %d.%02X error %d (error code 0x%04X)\n",
                     __func__, MemAddress, hi2c_index(hi2c), DevAddress, ret, hi2c->ErrorCode);
        return false;
    }
    int32_t status = wait_it_sem(hi2c, millisec);
    if (status != osOK) {
        log_printf(LOG_WARNING, "%s (0x%04X) I2C %d.%02X timeout\n", __func__, MemAddress, hi2c_index(hi2c), DevAddress);
        HAL_I2C_Master_Abort_IT(hi2c, DevAddress);
        i2c_driver_reset_internal(hi2c);
        return false;
    }
    return true;
}

bool i2c_driver_mem_write_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        assert(ret != HAL_BUSY);
        if (hi2c->ErrorCode & (HAL_I2C_ERROR_AF | HAL_I2C_ERROR_TIMEOUT)) {
//            i2c_driver_reset_internal(hi2c);
            log_printf(LOG_WARNING, "%s (0x%04X): I2C %d.%02X timeout (error code 0x%04X)\n",
                       __func__, MemAddress, hi2c_index(hi2c), DevAddress, hi2c->ErrorCode);
            return false; // no acknowledge or timeout
        }
        log_printf(LOG_WARNING, "%s (0x%04X): I2C %d.%02X error %d (error code 0x%04X)\n",
                   __func__, MemAddress, hi2c_index(hi2c), DevAddress, ret, hi2c->ErrorCode);
        return false;

    }
    int32_t status = wait_it_sem(hi2c, millisec);
    if (status != osOK) {
        log_printf(LOG_WARNING, "%s (0x%04X) I2C %d.%02X timeout\n", __func__, MemAddress, hi2c_index(hi2c), DevAddress);
        HAL_I2C_Master_Abort_IT(hi2c, DevAddress);
        i2c_driver_reset_internal(hi2c);
        return false;
    }
    return true;
}
