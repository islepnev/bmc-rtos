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

#include <assert.h>
#include <stdio.h>

#include "bsp.h"
#include "bsp_pin_defs.h"
#include "gpio.h"
#include "i2c.h"
#include "log/log.h"
#include "vxsiic_hal.h"
#include "vxsiic_iic_driver.h"

enum { PCA9548_BASE_I2C_ADDRESS = 0x71 };
enum {
    PAYLOAD_BOARD_MCU_I2C_ADDRESS = 0x33,
    PAYLOAD_BOARD_IOEXP_I2C_ADDRESS = 0x41,
    PAYLOAD_BOARD_EEPROM_I2C_ADDRESS = 0x51
};

void vxsiic_reset_mux(BusInterface *bus)
{
#ifdef BOARD_TTVXS
    write_gpio_pin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  0);
    write_gpio_pin(I2C_RESET2_B_GPIO_Port,  I2C_RESET2_B_Pin,  1);
#endif
}

bool vxsiic_detect_mux(BusInterface *bus)
{
    for (int i=0; i<3; i++) {
        vxsiic_reset_mux(bus);
        uint8_t data;
        BusInterface bus2 = *bus;
        bus2.address = PCA9548_BASE_I2C_ADDRESS + i;
        if (!vxsiic_read(&bus2, &data, 1))
            return false;
    }
    return true;
}

bool vxsiic_mux_select(BusInterface *bus, uint8_t subdevice, uint8_t channel, bool enable)
{
    assert(subdevice < 3);
    assert(channel < 8);
    uint8_t data;
    data = enable ? (1 << channel) : 0; // enable channel
    BusInterface bus2 = *bus;
    bus2.address = PCA9548_BASE_I2C_ADDRESS + subdevice;
    return vxsiic_write(&bus2, &data, 1);
}

//bool vxsiic_get_pp_i2c_status(BusInterface *bus, uint8_t pp)
//{
//    HAL_I2C_StateTypeDef state = HAL_I2C_GetState(hi2c_handle(bus->bus_number));
//    if (state != HAL_I2C_STATE_READY) {
//        log_printf(LOG_CRIT, "%s (port %2d) I2C not ready: state %d\n", __func__, pp, state);
//        return false;
//    }
//    return true;
//}

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

bool vxsiic_read_pp_eeprom(BusInterface *bus, uint8_t pp, uint16_t reg, uint8_t *data)
{
    BusInterface bus2 = *bus;
    bus2.address = PAYLOAD_BOARD_EEPROM_I2C_ADDRESS;
    return vxsiic_mem_read(&bus2, reg, I2C_MEMADD_SIZE_16BIT, data, 1);
}

bool vxsiic_read_pp_ioexp(BusInterface *bus, uint8_t pp, uint8_t reg, uint8_t *data)
{
    BusInterface bus2 = *bus;
    bus2.address = PAYLOAD_BOARD_IOEXP_I2C_ADDRESS;
    return vxsiic_mem_read(&bus2, reg, I2C_MEMADD_SIZE_8BIT, data, 1);
}

bool vxsiic_read_pp_mcu_4(BusInterface *bus, uint8_t pp, uint16_t reg, uint32_t *data)
{
    BusInterface bus2 = *bus;
    bus2.address = PAYLOAD_BOARD_MCU_I2C_ADDRESS;
    enum {Size = 4};
    return vxsiic_mem_read(&bus2, reg, I2C_MEMADD_SIZE_16BIT, (uint8_t *)data, Size);
}

bool vxsiic_write_pp_mcu_4(BusInterface *bus, uint8_t pp, uint16_t reg, uint32_t data)
{
    BusInterface bus2 = *bus;
    bus2.address = PAYLOAD_BOARD_MCU_I2C_ADDRESS;
    enum {Size = 4};
    return vxsiic_mem_write(&bus2, reg, I2C_MEMADD_SIZE_16BIT, (uint8_t *)data, Size);
}
