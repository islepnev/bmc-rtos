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

#include "ad9516_spi_hal.h"

#include "gpio.h"
#include "spi.h"
#include "bus/spi_driver.h"
#include "bsp.h"
#include "bsp_pin_defs.h"
#include "logbuffer.h"
#include "cmsis_os.h"

static const int SPI_TIMEOUT_MS = 500;

static bool set_csb(int state)
{
    if (ad9516_spi.Init.NSS != SPI_NSS_SOFT)
        return true;
    GPIO_PinState write = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    write_gpio_pin(AD9516_CS_GPIO_Port, AD9516_CS_Pin, write);
    GPIO_PinState read = read_gpio_pin(AD9516_CS_GPIO_Port, AD9516_CS_Pin);
    if (write != read) {
        log_printf(LOG_CRIT, "AD9516_CS_B stuck %s", read ? "high": "low");
    }
    return write == read;
}

//HAL_StatusTypeDef ad9516_read1_simplex(uint16_t reg, uint8_t *data)
//{
//    enum {Size = 3};
//    uint8_t tx[Size];
//    uint8_t rx[Size];
//    reg &= 0x1FFF;
//    tx[0] = 0x80 | (reg >> 8);  // MSB first, bit 15 = read
//    tx[1] = reg & 0xFF;
//    tx[2] = 0;
//    set_csb(0);
//    volatile HAL_StatusTypeDef ret1 = HAL_SPI_Transmit(ad9516_spi, tx, 2, SPI_TIMEOUT_MS);
//    volatile HAL_StatusTypeDef ret2 = HAL_SPI_Receive(ad9516_spi, rx, 1, SPI_TIMEOUT_MS);
//    set_csb(1);
//    if (ret1 == HAL_OK && ret2 == HAL_OK) {
//        if (data) {
//            *data = rx[2];
//        }
//    } else {
//        Error_Handler();
//    }
//    return HAL_OK;
//}

HAL_StatusTypeDef ad9516_read1_duplex(uint16_t reg, uint8_t *data)
{
    enum {Size = 3};
    uint8_t tx[Size];
    uint8_t rx[Size];
    reg &= 0x1FFF;
    tx[0] = 0x80 | (reg >> 8);  // MSB first, bit 15 = read
    tx[1] = reg & 0xFF;
    tx[2] = 0;
    set_csb(0);
    volatile HAL_StatusTypeDef ret = spi_driver_tx_rx(&ad9516_spi, tx, rx, Size, SPI_TIMEOUT_MS);
    set_csb(1);
    if (ret == HAL_OK) {
        if (data) {
            *data = rx[2];
        }
    }
    return ret;
}

HAL_StatusTypeDef ad9516_write1_internal(uint16_t reg, uint8_t data)
{
    enum {Size = 3};
    uint8_t tx[Size];
    reg &= 0x1FFF;
    tx[0] = (reg >> 8) & 0x1F;  // MSB first
    tx[1] = reg & 0xFF;
    tx[2] = data;
    set_csb(0);
    volatile HAL_StatusTypeDef ret = spi_driver_tx(&ad9516_spi, tx, Size, SPI_TIMEOUT_MS);
    set_csb(1);
    return ret;
}

void ad9516_test_loop(void)
{
    uint16_t reg = 0x011; // r-counter lsb
    uint8_t data = 0x55;
    while (1) {
        //sdata = ~data;
        HAL_StatusTypeDef ret = ad9516_write1_internal(reg, data);
        uint8_t read = 0;
        ad9516_read1_duplex(reg, &read);
        osDelay(1);
    }
}

HAL_StatusTypeDef ad9516_read1(uint16_t reg, uint8_t *data)
{
    return ad9516_read1_duplex(reg, data);
}

HAL_StatusTypeDef ad9516_write1(uint16_t reg, uint8_t data)
{
    HAL_StatusTypeDef ret = ad9516_write1_internal(reg, data);
    ad9516_write1_internal(0x232, 1); // io update
    uint8_t readback = 0;
    ad9516_read1(reg, &readback);
    if ((reg != 0x232) && (readback != data)) {
        log_printf(LOG_ERR, "Error writing %04X: wrote %02X, read %02X", reg, data, readback);
        return HAL_ERROR;
    }
    return ret;
}

HAL_StatusTypeDef ad9516_write_config(uint8_t data)
{
    return ad9516_write1_internal(0, data);
}

static void ad9516_spi_abort(void)
{
    if (HAL_OK != HAL_SPI_Abort(&ad9516_spi))
        log_printf(LOG_ERR, "%s: HAL error", __func__);
}

void ad9516_enable_interface(void)
{
    if (IS_SPI_ALL_INSTANCE(ad9516_spi.Instance))
        ad9516_disable_interface();
    HAL_SPI_MspInit(&ad9516_spi);
}

void ad9516_disable_interface(void)
{
    ad9516_spi_abort();
    HAL_SPI_MspDeInit(&ad9516_spi);
}
