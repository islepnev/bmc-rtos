//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "dev_vxsiic.h"

#include <stdio.h>
#include "stm32f7xx_hal.h"
#include "i2c.h"
#include "dev_vxsiic_types.h"
#include "logbuffer.h"
#include "debug_helpers.h"

static const int I2C_TIMEOUT_MS = 100;

static I2C_HandleTypeDef * const hi2c = &hi2c1;

enum { PCA9548_BASE_I2C_ADDRESS = 0x71 };
enum {
    PAYLOAD_BOARD_MCU_I2C_ADDRESS = 0x33,
    PAYLOAD_BOARD_EEPROM_I2C_ADDRESS = 0x51
};

static void vxsiic_reset_i2c_master(void)
{
    __HAL_I2C_DISABLE(hi2c);
    __HAL_I2C_ENABLE(hi2c);
}

static void vxsiic_reset_mux(void)
{
    HAL_GPIO_WritePin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  GPIO_PIN_SET);
}

static HAL_StatusTypeDef vxsiic_detect_mux(void)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    for (int i=0; i<3; i++) {
        vxsiic_reset_i2c_master();
        vxsiic_reset_mux();
        uint8_t i2c_address= PCA9548_BASE_I2C_ADDRESS + i;
        ret = HAL_I2C_IsDeviceReady(hi2c, i2c_address << 1, Trials, I2C_TIMEOUT_MS);
        if (ret != HAL_OK) {
            if (hi2c->ErrorCode != HAL_I2C_ERROR_TIMEOUT)
                debug_printf("VXS IIC mux 0x%02X error %d\n", i2c_address, hi2c->ErrorCode);
            return ret;
        }
    }
    return ret;
}

//HAL_StatusTypeDef pca9548_read(uint8_t *data, int subdevice)
//{
//    HAL_StatusTypeDef ret;
//    uint8_t pData;
//    ret = HAL_I2C_Master_Receive(hi2c, (PCA9548_BASE_I2C_ADDRESS + subdevice) << 1, &pData, 1, I2C_TIMEOUT_MS);
//    if (ret == HAL_OK) {
//        if (data) {
//            *data = pData;
//        }
//    }
//    return ret;
//}

// valid slot range: 2..21
// slot 2 = array[0]
static const int map_slot_to_channel[VXSIIC_SLOTS] = {
    3, 2, 1, 1, 0, 5, 7, 6, 5, 2, 3, 4, 4, 7, 6, 0, 1, 0
};

static const int map_slot_to_subdevice[VXSIIC_SLOTS] = {
    1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 1
};

const char * vxsiic_map_slot_to_label[VXSIIC_SLOTS] = {
    "2", "3", "4", "5", "6", "7", "8", "9", "10",
    "13", "14", "15", "16", "17", "18", "19", "20", "21"
};

static HAL_StatusTypeDef vxsiic_select_pp(Dev_vxsiic *d, uint8_t pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    assert_param(pp < VXSIIC_SLOTS);
    if (pp >= VXSIIC_SLOTS)
        return HAL_ERROR;
    uint8_t channel = map_slot_to_channel[pp];
    uint8_t subdevice = map_slot_to_subdevice[pp];
    uint8_t data;
    data = 1 << channel; // enable channel
    ret = HAL_I2C_Master_Transmit(hi2c, (PCA9548_BASE_I2C_ADDRESS + subdevice) << 1, &data, 1, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        debug_printf("%s (%2d) failed\n", __func__, pp);
    }
    return ret;
}

static HAL_StatusTypeDef vxsiic_get_pp_i2c_status(Dev_vxsiic *d, uint8_t pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    HAL_I2C_StateTypeDef state = HAL_I2C_GetState(hi2c);
    if (state != HAL_I2C_STATE_READY) {
        debug_printf("%s (port %2d) I2C not ready: state %d\n", __func__, pp, state);
        ret = HAL_ERROR;
    }
//    uint32_t isr = hi2c->Instance->ISR;
//    debug_printf("ISR = %08X\n", isr);
    if (ret != HAL_OK) {
        debug_printf("%s (port %2d) failed\n", __func__, pp);
    }
    return ret;
}

void sprint_i2c_error(char *buf, size_t size, uint32_t code)
{
    snprintf(buf, size, "%08lX%s%s%s%s%s%s%s",
             code,
             (code &  HAL_I2C_ERROR_BERR) ? " BERR" : "",
             (code &  HAL_I2C_ERROR_ARLO) ? " ARLO" : "",
             (code &  HAL_I2C_ERROR_AF) ? " AF" : "",
             (code &  HAL_I2C_ERROR_OVR) ?  " OVR" : "",
             (code &  HAL_I2C_ERROR_DMA) ?  " DMA" : "",
             (code &  HAL_I2C_ERROR_TIMEOUT) ? " TIMEOUT" : "",
             (code &  HAL_I2C_ERROR_DMA_PARAM) ? " DMA_PARAM" : ""
                                                 );
}

static HAL_StatusTypeDef vxsiic_detect_pp_eeprom(Dev_vxsiic *d, uint8_t pp)
{
    HAL_StatusTypeDef ret;
    uint32_t Trials = 2;
    ret = HAL_I2C_IsDeviceReady(hi2c, PAYLOAD_BOARD_EEPROM_I2C_ADDRESS << 1, Trials, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode == HAL_I2C_ERROR_TIMEOUT)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
        return ret;
    }
    return ret;
}

static HAL_StatusTypeDef vxsiic_read_pp_eeprom(Dev_vxsiic *d, uint8_t pp, uint16_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 1};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hi2c, (PAYLOAD_BOARD_EEPROM_I2C_ADDRESS << 1) | 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode == HAL_I2C_ERROR_AF)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    }
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

static HAL_StatusTypeDef vxsiic_read_pp_mcu(Dev_vxsiic *d, uint8_t pp, uint16_t reg, uint8_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 1};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hi2c, (PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1) | 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode == HAL_I2C_ERROR_AF)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    }
    if (ret == HAL_OK) {
        if (data) {
            *data = pData[0];
        }
    }
    return ret;
}

static HAL_StatusTypeDef vxsiic_read_pp_mcu_4(Dev_vxsiic *d, uint8_t pp, uint16_t reg, uint32_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 4};
    uint8_t pData[Size];
    ret = HAL_I2C_Mem_Read(hi2c, (PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1) | 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode == HAL_I2C_ERROR_AF)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    }
    if (ret == HAL_OK) {
        if (data) {
            *data = ((uint32_t)pData[3] << 24)
                    | ((uint32_t)pData[2] << 16)
                    | ((uint32_t)pData[1] << 8)
                    | pData[0];
        }
    }
//    osDelay(100);
    return ret;
}

static HAL_StatusTypeDef vxsiic_write_pp_mcu(Dev_vxsiic *d, uint8_t pp, uint16_t reg, uint8_t data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 1};
    uint8_t pData[Size];
    pData[0] = data;
    ret = HAL_I2C_Mem_Write(hi2c, PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode == HAL_I2C_ERROR_AF)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    }
    return ret;
}

static HAL_StatusTypeDef vxsiic_write_pp_mcu_4(Dev_vxsiic *d, uint8_t pp, uint16_t reg, uint32_t data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    enum {Size = 4};
    uint8_t pData[Size];
    pData[3] = (data >> 24) & 0xFF;
    pData[2] = (data >> 16) & 0xFF;
    pData[1] = (data >> 8) & 0xFF;
    pData[0] = data & 0xFF;
    ret = HAL_I2C_Mem_Write(hi2c, PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1, reg, I2C_MEMADD_SIZE_16BIT, pData, Size, I2C_TIMEOUT_MS);
    if (ret != HAL_OK) {
        if (hi2c->ErrorCode == HAL_I2C_ERROR_AF)
            return ret;
        enum {size = 100};
        char buf[size];
        sprint_i2c_error(buf, size, hi2c->ErrorCode);
        debug_printf("%s (port %2d) failed: %s\n", __func__, pp, buf);
    }
//    osDelay(100);
    return ret;
}

DeviceStatus dev_vxsiic_detect(Dev_vxsiic *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    if (HAL_OK != vxsiic_detect_mux())
        status = DEVICE_FAIL;
    d->present = status;
    return d->present;
}

uint32_t make_test_data(uint32_t i)
{
    return ((i*4+3)<< 24) | ((i*4+2)<< 16) | ((i*4+1)<< 8) | (i*4+0);
}

HAL_StatusTypeDef dev_vxsiic_read_pp_eeprom(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint16_t addr = 0;
    uint8_t eeprom_data = 0;
    ret = vxsiic_read_pp_eeprom(d, pp, addr, &eeprom_data);
    if (HAL_OK != ret)
        return ret;
//    debug_printf("EEPROM at slot %2s [%04X] = %02X\n", map_slot_to_label[pp], addr, eeprom_data);
    if (eeprom_data != 0xFF) {
        debug_printf("EEPROM at slot %2s [%04X] = %02X\n", vxsiic_map_slot_to_label[pp], addr, eeprom_data);
        return HAL_ERROR;
    }
    return ret;
}

HAL_StatusTypeDef dev_vxsiic_test_pp_mcu_regs(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint32_t addr = 0;
    uint32_t data = 0;
    int count = 2;
    for (int i=0; i<count; i++) {
        addr = 1+i;
        data = make_test_data(i);
        ret = vxsiic_write_pp_mcu_4(d, pp, addr, data);
        if (HAL_OK != ret)
            goto err;
    }
    for (int i=0; i<count; i++) {
        addr = 1+i;
        ret = vxsiic_read_pp_mcu_4(d, pp, addr, &data);
        if (HAL_OK != ret)
            goto err;
        const uint32_t test_data = make_test_data(i);
        if (test_data != data) {
        }
    }
    return ret;
err:
    return ret;
}

HAL_StatusTypeDef dev_vxsiic_read_pp_mcu(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    vxsiic_slot_status_t *status = &d->status.slot[pp];
    uint32_t addr = 0;
    uint32_t data = 0;
    ret = vxsiic_read_pp_mcu_4(d, pp, addr, &status->magic);
    if (HAL_OK != ret)
        goto err;
    status->present = 1;
    for (int i=0; i<MCU_MAP_SIZE; i++) {
        addr = 1+i;
        ret = vxsiic_read_pp_mcu_4(d, pp, addr, &data);
        if (HAL_OK != ret)
            goto err;
        status->map[i] = data;
    }
    return ret;
err:
    {
        vxsiic_slot_status_t zz = {0};
        *status = zz;
    }
    return ret;
}

HAL_StatusTypeDef dev_vxsiic_read(Dev_vxsiic *d)
{
    HAL_StatusTypeDef ret = HAL_OK;
    for (int pp=0; pp<VXSIIC_SLOTS; pp++) {
        vxsiic_reset_i2c_master();
        vxsiic_reset_mux();
        ret = vxsiic_select_pp(d, pp);
        if (HAL_OK != ret) {
            return ret;
        }
        ret = vxsiic_get_pp_i2c_status(d, pp);
        if (HAL_OK != ret) {
            continue;
        }
        if (HAL_OK != dev_vxsiic_read_pp_eeprom(d, pp)) {
            continue;
        }
        if (HAL_OK != dev_vxsiic_read_pp_mcu(d, pp)) {
            continue;
        }
    }
    return HAL_OK;
}
