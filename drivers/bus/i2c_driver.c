/*
**    Generic interrupt mode IIC driver
**
**    Copyright 2019-2020 Ilja Slepnev
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
#include "impl/i2c_driver_impl.h"
#include "impl/i2c_driver_util.h"

void i2c_driver_init(void)
{
    if (! i2c_driver_util_init())
        Error_Handler();
}

bool i2c_driver_get_master_ready(struct __I2C_HandleTypeDef *hi2c)
{
    return  HAL_I2C_STATE_READY == HAL_I2C_GetState(hi2c);
}

// Device-locked functions

bool i2c_driver_detect(struct __I2C_HandleTypeDef *hi2c, uint16_t deviceAddr, uint32_t Trials, uint32_t millisec)
{
    return i2c_driver_detect_internal(hi2c, deviceAddr, Trials, millisec);
}

bool i2c_driver_read(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    int dev_index = hi2c_index(hi2c);
    if (osOK != wait_dev_sem(dev_index, osWaitForever))
        return false;
    bool ret = i2c_driver_read_internal(hi2c, DevAddress, pData, Size, millisec);
    release_dev_sem(dev_index);
    return ret;
}

bool i2c_driver_write(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    int dev_index = hi2c_index(hi2c);
    if (osOK != wait_dev_sem(dev_index, osWaitForever))
        return false;
    bool ret = i2c_driver_write_internal(hi2c, DevAddress, pData, Size, millisec);
    release_dev_sem(dev_index);
    return ret;
}

bool i2c_driver_mem_read(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    int dev_index = hi2c_index(hi2c);
    if (osOK != wait_dev_sem(dev_index, osWaitForever))
        return false;
    bool ret = i2c_driver_mem_read_internal(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, millisec);
    release_dev_sem(dev_index);
    return ret;
}

bool i2c_driver_mem_write(struct __I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t millisec)
{
    int dev_index = hi2c_index(hi2c);
    if (osOK != wait_dev_sem(dev_index, osWaitForever))
        return false;
    bool ret = i2c_driver_mem_write_internal(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, millisec);
    release_dev_sem(dev_index);
    return ret;
}
