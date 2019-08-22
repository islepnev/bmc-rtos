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
#include "powermon_i2c_driver.h"

#include "i2c.h"
#include "bsp.h"
#include "cmsis_os.h"

struct __I2C_HandleTypeDef * const hi2c_sensors = &hi2c4;

static const int I2C_TIMEOUT_MS = 10;

void pm_sensor_reset_i2c_master(void)
{
    __HAL_I2C_DISABLE(hi2c_sensors);
    __HAL_I2C_ENABLE(hi2c_sensors);
}

HAL_StatusTypeDef powermon_i2c_detect(uint16_t deviceAddr, uint32_t Trials)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_IsDeviceReady(hi2c_sensors, deviceAddr, Trials, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef powermon_i2c_mem_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_Mem_Read(hi2c_sensors, DevAddress, MemAddress, MemAddSize, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef powermon_i2c_mem_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_Mem_Write(hi2c_sensors, DevAddress, MemAddress, MemAddSize, pData, Size, I2C_TIMEOUT_MS);
    return ret;
}

void powermon_I2C_MasterTxCpltCallback(void)
{

}

void powermon_I2C_MasterRxCpltCallback(void)
{

}

void powermon_HAL_I2C_MemTxCpltCallback(void)
{

}

void powermon_HAL_I2C_MemRxCpltCallback(void)
{

}

void powermon_HAL_I2C_ErrorCallback(void)
{

}

void powermon_HAL_I2C_AbortCpltCallback(void)
{

}
