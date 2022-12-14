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
#include "devicebase.h"
#include "error_handler.h"
#include "i2c.h"
#include "i2c_driver_util.h"
#include "log/log.h"
#include "stm32_hal.h"
#include "stm32_ll.h"

void i2c_driver_log_error(const char *title, BusInterface *bus)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    if (bus->bus_number == 1 && hi2c->ErrorCode == HAL_I2C_ERROR_AF)
        return; // FIXME: no message for vxsiicm
    log_printf(LOG_WARNING, "I2C %d.%02X %s '%s' %s:%s%s%s%s%s%s%s%s%s (error code 0x%04X)\n",
               bus->bus_number, bus->address,
               bus->dev ? device_class_str(bus->dev->device_class) : "",
               bus->dev ? bus->dev->name : "",
               title,
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

void i2c_driver_reset_internal(BusInterface *bus)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    __HAL_I2C_DISABLE(hi2c);
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    hi2c->State = HAL_I2C_STATE_READY;
    hi2c->PreviousState = HAL_I2C_MODE_NONE;
    hi2c->Mode = HAL_I2C_MODE_NONE;
    __HAL_I2C_ENABLE(hi2c);
}

static bool i2c_driver_wait_complete(const char *title, BusInterface *bus, uint32_t millisec)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    const uint16_t DevAddress = bus->address << 1;
    int32_t status = i2c_driver_wait_it_sem(hi2c, millisec);
    if (status != osOK) {
        log_printf(LOG_CRIT, "I2C %d.%02X %s '%s' %s: timeout\n",
                   bus->bus_number, bus->address,
                   bus->dev ? device_class_str(bus->dev->device_class) : "",
                   bus->dev ? bus->dev->name : "",
                   title);
        HAL_I2C_Master_Abort_IT(hi2c, DevAddress);
        i2c_driver_reset_internal(bus);
        return false;
    }
    if (! i2c_driver_is_transfer_ok(hi2c)) {
        log_printf(LOG_CRIT, "I2C %d.%02X %s '%s' %s: transfer failed\n",
                   bus->bus_number, bus->address,
                   bus->dev ? device_class_str(bus->dev->device_class) : "",
                   bus->dev ? bus->dev->name : "",
                   title);
        return false;
    }
    return true;
}

static bool i2c_driver_check_hal_ret(const char *title, BusInterface *bus, int ret)
{
    if (HAL_OK == ret)
        return true;
    assert(ret != HAL_BUSY);
    i2c_driver_log_error(title, bus);
    return false;
}

static bool i2c_driver_before_hal_call(const char *title, BusInterface *bus)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    if (hi2c->State == HAL_I2C_STATE_RESET) {
        log_printf(LOG_CRIT, "I2C %d.%02X %s '%s' %s: peripheral not initialized\n",
                   bus->bus_number, bus->address,
                   bus->dev ? device_class_str(bus->dev->device_class) : "",
                   bus->dev ? bus->dev->name : "",
                   title);
        return false;
    }
    assert(hi2c->State == HAL_I2C_STATE_READY);
    i2c_driver_clear_transfer_error(hi2c);
    if (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance)) {
        log_printf(LOG_CRIT, "I2C %d.%02X %s '%s' %s: bus busy\n",
                   bus->bus_number, bus->address,
                   bus->dev ? device_class_str(bus->dev->device_class) : "",
                   bus->dev ? bus->dev->name : "",
                   title);
        i2c_driver_reset_internal(bus);
        return false;
    }
    return true;
}

static bool i2c_driver_after_hal_call(const char *title, BusInterface *bus, int ret, uint32_t millisec)
{
    return i2c_driver_check_hal_ret(title, bus, ret) &&
           i2c_driver_wait_complete(title, bus, millisec);
}

bool i2c_driver_bus_ready_internal(BusInterface *bus)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    return 0 == LL_I2C_IsActiveFlag_BUSY(hi2c->Instance);
}

bool i2c_driver_detect_internal(BusInterface *bus, uint32_t Trials, uint32_t millisec)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    const uint16_t DevAddress = (bus->address << 1) | 1; // set read bit
    if (!i2c_driver_before_hal_call("detect", bus))
        return false;
    HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(hi2c, DevAddress, Trials, millisec);
    if (HAL_OK == ret)
        return true;
    if (hi2c->ErrorCode != HAL_I2C_ERROR_AF && hi2c->ErrorCode != HAL_I2C_ERROR_TIMEOUT)
        i2c_driver_log_error("detect", bus);
    return false;
    // return i2c_driver_check_hal_ret("detect", hi2c, DevAddress, ret);
}

bool i2c_driver_read_internal(BusInterface *bus, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    const uint16_t DevAddress = (bus->address << 1) | 1; // set read bit
    if (!i2c_driver_before_hal_call("read", bus))
        return false;
    HAL_StatusTypeDef ret = HAL_I2C_Master_Receive_IT(hi2c, DevAddress, pData, Size);
    return i2c_driver_after_hal_call("read", bus, ret, millisec);
}

bool i2c_driver_write_internal(BusInterface *bus, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    const uint16_t DevAddress = bus->address << 1;
    if (!i2c_driver_before_hal_call("write", bus))
        return false;
    HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit_IT(hi2c, DevAddress, pData, Size);
    return i2c_driver_after_hal_call("write", bus, ret, millisec);
}

bool i2c_driver_mem_read_internal(BusInterface *bus, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    const uint16_t DevAddress = (bus->address << 1) | 1; // set read bit
    if (!i2c_driver_before_hal_call("mem read", bus))
        return false;
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    return i2c_driver_after_hal_call("mem read", bus, ret, millisec);
}

bool i2c_driver_mem_write_internal(BusInterface *bus, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    struct __I2C_HandleTypeDef *hi2c = hi2c_handle(bus->bus_number);
    const uint16_t DevAddress = bus->address << 1;
    if (!i2c_driver_before_hal_call("mem write", bus))
        return false;
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write_IT(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    return i2c_driver_after_hal_call("mem write", bus, ret, millisec);
}
