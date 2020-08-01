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

#include "vxsiic_hal.h"

#include <stdio.h>

#include "bsp.h"
#include "bsp_pin_defs.h"
#include "gpio.h"
#include "i2c.h"
#include "log/logbuffer.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_i2c.h"
#include "vxsiic_hal.h"
#include "vxsiic_iic_driver.h"

enum { PCA9548_BASE_I2C_ADDRESS = 0x71 };
enum {
    PAYLOAD_BOARD_MCU_I2C_ADDRESS = 0x33,
    PAYLOAD_BOARD_IOEXP_I2C_ADDRESS = 0x41,
    PAYLOAD_BOARD_EEPROM_I2C_ADDRESS = 0x51
};

void vxsiic_reset_mux(void)
{
    write_gpio_pin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  0);
    write_gpio_pin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  1);
}

bool vxsiic_detect_mux(void)
{
    for (int i=0; i<3; i++) {
        vxsiic_reset_mux();
        uint8_t i2c_address= PCA9548_BASE_I2C_ADDRESS + i;
        uint8_t data;
        if (!vxsiic_read(i2c_address << 1, &data, 1))
            return false;
    }
    return true;
}

bool vxsiic_mux_select(uint8_t subdevice, uint8_t channel)
{
    assert_param(subdevice < 3);
    assert_param(channel < 8);
    uint8_t data;
    data = 1 << channel; // enable channel
    uint8_t i2c_address = PCA9548_BASE_I2C_ADDRESS + subdevice;
    return vxsiic_write(i2c_address << 1, &data, 1);
}

bool vxsiic_get_pp_i2c_status(uint8_t pp)
{
    HAL_I2C_StateTypeDef state = HAL_I2C_GetState(&vxsiic_hi2c);
    if (state != HAL_I2C_STATE_READY) {
        log_printf(LOG_CRIT, "%s (port %2d) I2C not ready: state %d\n", __func__, pp, state);
        return false;
    }
    return true;
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

bool vxsiic_read_pp_eeprom(uint8_t pp, uint16_t reg, uint8_t *data)
{
    uint8_t dev_address = (PAYLOAD_BOARD_EEPROM_I2C_ADDRESS << 1) | 1;
    return vxsiic_mem_read(dev_address, reg, I2C_MEMADD_SIZE_16BIT, data, 1);
}

bool vxsiic_read_pp_ioexp(uint8_t pp, uint8_t reg, uint8_t *data)
{
    uint8_t dev_address = (PAYLOAD_BOARD_IOEXP_I2C_ADDRESS << 1) | 1;
    return vxsiic_mem_read(dev_address, reg, I2C_MEMADD_SIZE_8BIT, data, 1);
}

bool vxsiic_read_pp_mcu_4(uint8_t pp, uint16_t reg, uint32_t *data)
{
    enum {Size = 4};
    uint8_t dev_address = (PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1) | 1;
    return vxsiic_mem_read(dev_address, reg, I2C_MEMADD_SIZE_16BIT, (uint8_t *)data, Size);
}

bool vxsiic_write_pp_mcu_4(uint8_t pp, uint16_t reg, uint32_t data)
{
    enum {Size = 4};
    return vxsiic_mem_write(PAYLOAD_BOARD_MCU_I2C_ADDRESS << 1, reg, I2C_MEMADD_SIZE_16BIT, (uint8_t *)data, Size);
}
