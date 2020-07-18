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
#include "dev_sfpiic_driver.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_i2c.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "debug_helpers.h"
#include "bsp_pin_defs.h"
#include "bsp.h"

static const int SFPI2C_TIMEOUT_MS = 25;
static const int I2C_TIMEOUT_MS = 10;

osSemaphoreId sfpiic_semaphore;                         // Semaphore ID
osSemaphoreDef(sfpiic_semaphore);                       // Semaphore definition

void sfpiic_master_reset(void)
{
    __HAL_I2C_DISABLE(&hi2c_sfpiic);
    hi2c_sfpiic.ErrorCode = HAL_I2C_ERROR_NONE;
    hi2c_sfpiic.State = HAL_I2C_STATE_READY;
    hi2c_sfpiic.PreviousState = HAL_I2C_MODE_NONE;
    hi2c_sfpiic.Mode = HAL_I2C_MODE_NONE;
    __HAL_I2C_ENABLE(&hi2c_sfpiic);
}

void sfpiic_switch_reset(void)
{
    HAL_GPIO_WritePin(I2C_RESET3_B_GPIO_Port,  I2C_RESET3_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_RESET3_B_GPIO_Port,  I2C_RESET3_B_Pin,  GPIO_PIN_SET);
}

HAL_StatusTypeDef sfpiic_device_detect(uint16_t addr)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    uint32_t Trials = 2;
    ret = HAL_I2C_IsDeviceReady(&hi2c_sfpiic, addr << 1, Trials, I2C_TIMEOUT_MS);
    return ret;
}
HAL_StatusTypeDef sfpiic_switch_set_channel(uint8_t channel)
{
    assert_param(channel < 8);
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t data;
    data = (uint8_t)(1 << channel); // enable channel
    ret = sfpiic_write(&data, 1);
    return ret;
}

void sfpiic_I2C_MasterTxCpltCallback(void)
{
    osSemaphoreRelease(sfpiic_semaphore);
}

void sfpiic_I2C_MasterRxCpltCallback(void)
{
    osSemaphoreRelease(sfpiic_semaphore);
}

void sfpiic_HAL_I2C_MemTxCpltCallback(void)
{
    osSemaphoreRelease(sfpiic_semaphore);
}

void sfpiic_HAL_I2C_MemRxCpltCallback(void)
{
    osSemaphoreRelease(sfpiic_semaphore);
}

void sfpiic_HAL_I2C_ErrorCallback(void)
{
    debug_printf("%s I2C error, code %d\n", __func__, hi2c_sfpiic.ErrorCode);
    // reinitialize I2C
    sfpiic_master_reset();
    osSemaphoreRelease(sfpiic_semaphore);
}

void sfpiic_HAL_I2C_AbortCpltCallback(void)
{
    debug_printf("%s\n", __func__);
    osSemaphoreRelease(sfpiic_semaphore);
}

void sfpiic_init(void)
{
    // Create and take the semaphore
    sfpiic_semaphore = osSemaphoreCreate(osSemaphore(sfpiic_semaphore), 1);
    osStatus status = (osStatus)osSemaphoreWait(sfpiic_semaphore, osWaitForever);
    if (status != osOK) {
        debug_printf("%s(%02X): i2c timeout\n", __func__, 1);
    }
}

HAL_StatusTypeDef sfpiic_read(uint8_t *pData, uint16_t Size)
{
    uint16_t addr = (PCA9548_BASE_I2C_ADDRESS<<1)|1;
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_I2C_Master_Receive(&hi2c_sfpiic, addr, pData, Size, SFPI2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        debug_printf("%s(%02X): i2c error %d, %d\n", __func__, addr, ret, hi2c_sfpiic.ErrorCode);
    }
    return ret;
    ret = HAL_I2C_Master_Receive_IT(&hi2c_sfpiic, addr, pData, Size);
    if (ret != HAL_OK) {
        debug_printf("%s(%02X): i2c error %d, %d\n", __func__, addr, ret, hi2c_sfpiic.ErrorCode);
    }
    osStatus status = (osStatus)osSemaphoreWait(sfpiic_semaphore, SFPI2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s(%02X): i2c timeout\n", __func__, addr);
        return HAL_TIMEOUT;
    }
    return ret;
}

HAL_StatusTypeDef sfpiic_write(uint8_t *pData, uint16_t Size)
{
    uint16_t addr = PCA9548_BASE_I2C_ADDRESS<<1;
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_I2C_Master_Transmit(&hi2c_sfpiic, addr, pData, Size, SFPI2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        debug_printf("%s(%02X): i2c error %d, %d\n", __func__, addr, ret, hi2c_sfpiic.ErrorCode);
    }
    return ret;
    ret = HAL_I2C_Master_Transmit_IT(&hi2c_sfpiic, addr, pData, Size);
    if (ret != HAL_OK) {
        debug_printf("%s(%02X): i2c error %d, %d\n", __func__, addr, ret, hi2c_sfpiic.ErrorCode);
    }
    osStatus status = (osStatus)osSemaphoreWait(sfpiic_semaphore, 1);
    if (status != osOK) {
        debug_printf("%s(%02X): i2c timeout\n", __func__, addr);
        return HAL_TIMEOUT;
    }
    return ret;
}

HAL_StatusTypeDef sfpiic_mem_read(uint16_t addr, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret = HAL_OK;
    addr  = (uint16_t)((addr<<1)|1);
    ret = HAL_I2C_Mem_Read(&hi2c_sfpiic, addr, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, SFPI2C_TIMEOUT_MS);
//    ret = HAL_I2C_Mem_Read_IT(sfpiic_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (hi2c_sfpiic.ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, empty slot
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (dev_addr %02X, mem_addr 0x%04X): HAL code %d, I2C code %d\n", __func__, addr, MemAddress, ret, hi2c_sfpiic.ErrorCode);
            return ret;
        }
    }
    return ret;
    osStatus status = (osStatus)osSemaphoreWait(sfpiic_semaphore, SFPI2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (dev_addr %02X, mem_addr 0x%04X) timeout\n", __func__, addr, MemAddress);
        return HAL_TIMEOUT;
    }
//    //    if (ret != HAL_OK) {
//    //        if (sfpiic_hi2c->ErrorCode & HAL_I2C_ERROR_AF)
//    //            return ret;
//    //        enum {size = 100};
//    //        char buf[size];
//    //        sprint_i2c_error(buf, size, sfpiic_hi2c->ErrorCode);
//    //        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
//    //    }
//    //    if (ret == HAL_OK) {
//    //    }
//    //    return ret;
    return HAL_OK;
}
HAL_StatusTypeDef sfpiic_mem_read16(uint16_t addr, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret = HAL_OK;
    addr  = (uint16_t)((addr<<1)|1);
    ret = HAL_I2C_Mem_Read(&hi2c_sfpiic, addr, MemAddress, I2C_MEMADD_SIZE_16BIT, pData, Size, SFPI2C_TIMEOUT_MS);
//    ret = HAL_I2C_Mem_Read_IT(sfpiic_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (hi2c_sfpiic.ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, empty slot
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (dev_addr %02X, mem_addr 0x%04X): HAL code %d, I2C code %d\n", __func__, addr, MemAddress, ret, hi2c_sfpiic.ErrorCode);
            return ret;
        }
    }
    return ret;
    osStatus status = (osStatus)osSemaphoreWait(sfpiic_semaphore, SFPI2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (dev_addr %02X, mem_addr 0x%04X) timeout\n", __func__, addr, MemAddress);
        return HAL_TIMEOUT;
    }
//    //    if (ret != HAL_OK) {
//    //        if (sfpiic_hi2c->ErrorCode & HAL_I2C_ERROR_AF)
//    //            return ret;
//    //        enum {size = 100};
//    //        char buf[size];
//    //        sprint_i2c_error(buf, size, sfpiic_hi2c->ErrorCode);
//    //        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
//    //    }
//    //    if (ret == HAL_OK) {
//    //    }
//    //    return ret;
    return HAL_OK;
}

HAL_StatusTypeDef sfpiic_mem_write(uint16_t addr, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret = HAL_OK;
    addr  = (uint16_t)((addr<<1)|1);
    ret = HAL_I2C_Mem_Write(&hi2c_sfpiic, addr, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, SFPI2C_TIMEOUT_MS);
//    ret = HAL_I2C_Mem_Write_IT(sfpiic_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (hi2c_sfpiic.ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, MCU not loaded
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (dev_addr %02X, mem_addr 0x%04X): HAL code %d, I2C code %d\n", __func__, addr, MemAddress, ret, hi2c_sfpiic.ErrorCode);
            return ret;
        }
    }
    return ret;
    osStatus status = (osStatus)osSemaphoreWait(sfpiic_semaphore, SFPI2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (dev_addr %02X, mem_addr 0x%04X) timeout\n", __func__, addr, MemAddress);
        return HAL_TIMEOUT;
    }
    return HAL_OK;
}

HAL_StatusTypeDef sfpiic_mem_write16(uint16_t addr, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    HAL_StatusTypeDef ret = HAL_OK;
    addr  = (uint16_t)((addr<<1)|1);
    ret = HAL_I2C_Mem_Write(&hi2c_sfpiic, addr, MemAddress, I2C_MEMADD_SIZE_16BIT, pData, Size, SFPI2C_TIMEOUT_MS);
//    ret = HAL_I2C_Mem_Write_IT(sfpiic_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    if (ret != HAL_OK) {
        if (hi2c_sfpiic.ErrorCode & HAL_I2C_ERROR_AF) {
            // no acknowledge, MCU not loaded
            return HAL_TIMEOUT;
        } else {
            debug_printf("%s (dev_addr %02X, mem_addr 0x%04X): HAL code %d, I2C code %d\n", __func__, addr, MemAddress, ret, hi2c_sfpiic.ErrorCode);
            return ret;
        }
    }
    return ret;
    osStatus status = (osStatus)osSemaphoreWait(sfpiic_semaphore, SFPI2C_TIMEOUT_MS);
    if (status != osOK) {
        debug_printf("%s (dev_addr %02X, mem_addr 0x%04X) timeout\n", __func__, addr, MemAddress);
        return HAL_TIMEOUT;
    }
    return HAL_OK;
}


HAL_StatusTypeDef sfpiic_get_ch_i2c_status(uint8_t ch)
{
    HAL_StatusTypeDef ret = HAL_OK;
    HAL_I2C_StateTypeDef state = HAL_I2C_GetState(&hi2c_sfpiic);
    if (state != HAL_I2C_STATE_READY) {
        debug_printf("%s (port %2d) I2C not ready: state %d\n", __func__, ch, state);
        ret = HAL_ERROR;
    }
    //    uint32_t isr = hi2c->Instance->ISR;
    //    debug_printf("ISR = %08X\n", isr);
//    if (ret != HAL_OK) {
//        debug_printf("%s (port %2d) failed\n", __func__, pp);
//    }
    return ret;
}
