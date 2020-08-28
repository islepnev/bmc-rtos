/*
**    Generic interrupt mode I2C driver
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

#include <assert.h>

#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "error_handler.h"
#include "i2c.h"
#include "i2c_driver_util.h"
#include "log/log.h"
#include "stm32_hal.h"
#include "stm32_ll.h"

void i2c_driver_log_error(const char *title, struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress)
{
    BusIndex index = hi2c_index(hi2c);
    if (index == 1 && hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        return; // no message for vxsiicm
    log_printf(LOG_WARNING, "%s: I2C %d.%02X %s%s%s%s%s%s%s%s%s (error code 0x%04X)\n",
               title, hi2c_index(hi2c), DevAddress >> 1,
               (hi2c->ErrorCode & HAL_I2C_ERROR_BERR)          ? " BERR" : "",
               (hi2c->ErrorCode & HAL_I2C_ERROR_ARLO)          ? " ARLO" : "",
               (hi2c->ErrorCode & HAL_I2C_ERROR_AF)            ? " ACKF" : "",
               (hi2c->ErrorCode & HAL_I2C_ERROR_OVR)           ? " OVR" : "",
               (hi2c->ErrorCode & HAL_I2C_ERROR_DMA)           ? " DMA_transfer" : "",
               (hi2c->ErrorCode & HAL_I2C_ERROR_TIMEOUT)       ? " timeout" : "",
               (hi2c->ErrorCode & HAL_I2C_ERROR_SIZE)          ? " size_error" : "",
               (hi2c->ErrorCode & HAL_I2C_ERROR_DMA_PARAM)     ? " DMA_parameter_error" : "",
               (hi2c->ErrorCode & HAL_I2C_ERROR_INVALID_PARAM) ? " invalid_parameters" : "",
               hi2c->ErrorCode);
}

void i2c_driver_reset_internal(struct __I2C_HandleTypeDef *hi2c)
{
    __HAL_I2C_DISABLE(hi2c);
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->PreviousState = HAL_I2C_MODE_NONE;
    hi2c->Mode = HAL_I2C_MODE_NONE;
    __HAL_I2C_ENABLE(hi2c);
}

static bool i2c_driver_wait_complete(const char *title, struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t millisec)
{
    int32_t status = i2c_driver_wait_it_sem(hi2c, millisec);
    if (status != osOK) {
        log_printf(LOG_CRIT, "%s: I2C %d.%02X timeout\n", title, hi2c_index(hi2c), DevAddress >> 1);
        HAL_I2C_Master_Abort_IT(hi2c, DevAddress);
        i2c_driver_reset_internal(hi2c);
        return false;
    }
    if (! i2c_driver_is_transfer_ok(hi2c)) {
        log_printf(LOG_CRIT, "%s: I2C %d.%02X transfer failed\n", title, hi2c_index(hi2c), DevAddress >> 1);
        return false;
    }
    return true;
}

static bool i2c_driver_check_hal_ret(const char *title, struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, int ret)
{
    if (HAL_OK == ret)
        return true;
    assert(ret != HAL_BUSY);
    i2c_driver_log_error(title, hi2c, DevAddress);
    return false;
}

static bool i2c_driver_before_hal_call(const char *title, struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress)
{
    if (hi2c->State == HAL_I2C_STATE_RESET) {
        log_printf(LOG_CRIT, "%s: I2C %d not initialized\n",
                   title, hi2c_index(hi2c));
        return false;
    }
    assert(hi2c->State == HAL_I2C_STATE_READY);
    i2c_driver_clear_transfer_error(hi2c);
    if (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance)) {
        log_printf(LOG_CRIT, "%s: I2C %d.%02X bus busy\n",
                   title, hi2c_index(hi2c), DevAddress >> 1);
        i2c_driver_reset_internal(hi2c);
        return false;
    }
    return true;
}

static bool i2c_driver_after_hal_call(const char *title, struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, int ret, uint32_t millisec)
{
    return i2c_driver_check_hal_ret(title, hi2c, DevAddress, ret) &&
           i2c_driver_wait_complete(title, hi2c, DevAddress, millisec);
}

bool i2c_driver_bus_ready_internal(struct __I2C_HandleTypeDef *hi2c)
{
    return 0 == LL_I2C_IsActiveFlag_BUSY(hi2c->Instance);
}

bool i2c_driver_detect_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t millisec)
{
    if (!i2c_driver_before_hal_call(__func__, hi2c, DevAddress))
        return false;
    DevAddress |= 1; // set read bit
    HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(hi2c, DevAddress, Trials, millisec);
    if (HAL_OK == ret)
        return true;
    if (hi2c->ErrorCode != HAL_I2C_ERROR_AF && hi2c->ErrorCode != HAL_I2C_ERROR_TIMEOUT)
        i2c_driver_log_error(__func__, hi2c, DevAddress);
    return false;
    // return i2c_driver_check_hal_ret(__func__, hi2c, DevAddress, ret);
}

bool i2c_driver_read_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    if (!i2c_driver_before_hal_call(__func__, hi2c, DevAddress))
        return false;
    DevAddress |= 1; // set read bit
    HAL_StatusTypeDef ret = HAL_I2C_Master_Receive_IT(hi2c, DevAddress, pData, Size);
    return i2c_driver_after_hal_call(__func__, hi2c, DevAddress, ret, millisec);
}

bool i2c_driver_write_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    if (!i2c_driver_before_hal_call(__func__, hi2c, DevAddress))
        return false;
    DevAddress &= ~1; // clear read bit
    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit_IT(hi2c, DevAddress, pData, Size);
    return i2c_driver_after_hal_call(__func__, hi2c, DevAddress, ret, millisec);
}

bool i2c_driver_mem_read_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    if (!i2c_driver_before_hal_call(__func__, hi2c, DevAddress))
        return false;
    DevAddress |= 1; // set read bit
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    return i2c_driver_after_hal_call(__func__, hi2c, DevAddress, ret, millisec);
}

bool i2c_driver_mem_write_internal(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    if (!i2c_driver_before_hal_call(__func__, hi2c, DevAddress))
        return false;
    DevAddress &= ~1; // clear read bit
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    return i2c_driver_after_hal_call(__func__, hi2c, DevAddress, ret, millisec);
}
