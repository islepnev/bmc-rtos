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
#include "pll_i2c_driver.h"

#include "i2c.h"
#include "bsp.h"
#include "debug_helpers.h"
#include "cmsis_os.h"

static const int I2C_TIMEOUT_MS = 100;

osSemaphoreId pll_i2c_sem;                         // Semaphore ID
osSemaphoreDef(pll_i2c_sem);                       // Semaphore definition

void pll_i2c_init(void)
{
    // Create and take the semaphore
    pll_i2c_sem = osSemaphoreCreate(osSemaphore(pll_i2c_sem), 1);
    osSemaphoreWait(pll_i2c_sem, osWaitForever);
}

void pll_i2c_reset_master(struct __I2C_HandleTypeDef *hPll)
{
    __HAL_I2C_DISABLE(hPll);
    hPll->ErrorCode = HAL_I2C_ERROR_NONE;
    hPll->State = HAL_I2C_STATE_READY;
    hPll->PreviousState = HAL_I2C_MODE_NONE;
    hPll->Mode = HAL_I2C_MODE_NONE;
    __HAL_I2C_ENABLE(hPll);
}

void pll_I2C_MasterTxCpltCallback(void)
{
    osSemaphoreRelease(pll_i2c_sem);
}

void pll_I2C_MasterRxCpltCallback(void)
{
    osSemaphoreRelease(pll_i2c_sem);
}

void pll_HAL_I2C_MemTxCpltCallback(void)
{
    osSemaphoreRelease(pll_i2c_sem);
}

void pll_HAL_I2C_MemRxCpltCallback(void)
{
    osSemaphoreRelease(pll_i2c_sem);
}

void pll_HAL_I2C_ErrorCallback(void)
{
    debug_printf("%s I2C error, code %d\n", __func__, hPll->ErrorCode);
    // reinitialize I2C
    pll_i2c_reset_master(hPll);
    osSemaphoreRelease(pll_i2c_sem);
}

void pll_HAL_I2C_AbortCpltCallback(void)
{
    debug_printf("%s\n", __func__);
    osSemaphoreRelease(pll_i2c_sem);
}

HAL_StatusTypeDef pll_i2c_detect(uint16_t deviceAddr, uint32_t Trials)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_IsDeviceReady(hPll, deviceAddr, Trials, I2C_TIMEOUT_MS);
    return ret;
}

HAL_StatusTypeDef pll_i2c_mem_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_Mem_Read_IT(hPll, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (hPll->ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, empty slot
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (%02X, 0x%04X): HAL code %d, I2C code %d\n", __func__, DevAddress, MemAddress, ret, hPll->ErrorCode);
            return ret;
        }
    }
    osStatus status = osSemaphoreWait(pll_i2c_sem, I2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (%02X, 0x%04X) timeout\n", __func__, DevAddress, MemAddress);
        return HAL_TIMEOUT;
    }
    return ret;
}

HAL_StatusTypeDef pll_i2c_mem_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret;
    ret = HAL_I2C_Mem_Write_IT(hPll, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (hPll->ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, empty slot
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (%02X, 0x%04X): HAL code %d, I2C code %d\n", __func__, DevAddress, MemAddress, ret, hPll->ErrorCode);
            return ret;
        }
    }
    osStatus status = osSemaphoreWait(pll_i2c_sem, I2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (%02X, 0x%04X) timeout\n", __func__, DevAddress, MemAddress);
        return HAL_TIMEOUT;
    }
    return ret;
}
